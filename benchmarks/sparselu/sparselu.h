#ifndef SPARSELU_H
#define SPARSELU_H

#include <cmath>
#include <omp.h>

template <typename T, typename AT, typename AT2>
void Init(AT2 matrix, size_t size, size_t)
{
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
}

template <typename T, typename AT, typename AT2>
T Check(AT2 matrix, size_t size, size_t)
{
	T sum = 0;

	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
			sum += matrix[i][j];

	return sum;
}

//https://github.com/alcides/bots

/***********************************************************************
 * lu0: 
 **********************************************************************/

template<typename AT>
void lu0(AT diag, size_t size)
{
	size_t i, j, k;
	for (k=0; k<size; k++)
		for (i=k+1; i<size; i++)
		{
			diag[i*size+k] = diag[i*size+k] / diag[k*size+k];
			for (j=k+1; j<size; j++)
				diag[i*size+j] = diag[i*size+j] - diag[i*size+k] * diag[k*size+j];
		}
}

/***********************************************************************
 * bdiv: 
 **********************************************************************/
template<typename AT>
void bdiv(AT diag, AT row, size_t size)
{
	size_t i, j, k;
	for (i=0; i<size; i++)
		for (k=0; k<size; k++)
		{
			row[i*size+k] = row[i*size+k] / diag[k*size+k];
			for (j=k+1; j<size; j++)
				row[i*size+j] = row[i*size+j] - row[i*size+k]*diag[k*size+j];
		}
}
/***********************************************************************
 * bmod: 
 **********************************************************************/
template<typename AT>
void bmod(AT row, AT col, AT inner, size_t size)
{
	size_t i, j, k;
	for (i=0; i<size; i++)
		for (j=0; j<size; j++)
			for (k=0; k<size; k++)
				inner[i*size+j] = inner[i*size+j] - row[i*size+k]*col[k*size+j];
}
/***********************************************************************
 * fwd: 
 **********************************************************************/
template<typename AT>
void fwd(AT diag, AT col, size_t size)
{
	size_t i, j, k;
	for (j=0; j<size; j++)
		for (k=0; k<size; k++) 
			for (i=k+1; i<size; i++)
				col[i*size+j] = col[i*size+j] - diag[i*size+k]*col[k*size+j];
}

template<typename AT, typename AT2>
void sparselu_par_call(AT2 BENCH, size_t size, size_t inner_size)
{
	size_t ii, jj, kk;

	#pragma omp parallel num_threads(LOC_THREADS)
	#pragma omp single nowait
	#pragma omp task untied
	for (kk=0; kk<size; kk++) 
	{
		lu0<AT>(BENCH[kk*size+kk], inner_size);

		for (jj=kk+1; jj<size; jj++)
//			if (BENCH[kk*size+jj] != NULL)
				if((kk * 2 + jj * 3) % 10 == 1)
				#pragma omp task untied firstprivate(kk, jj) shared(BENCH)
				{
					fwd<AT>(BENCH[kk*size+kk], BENCH[kk*size+jj], inner_size);
				}

		for (ii=kk+1; ii<size; ii++) 
//			if (BENCH[ii*size+kk] != NULL)
				if((ii * 2 + kk * 3) % 10 == 1)
				#pragma omp task untied firstprivate(kk, ii) shared(BENCH)
				{
					bdiv<AT>(BENCH[kk*size+kk], BENCH[ii*size+kk], inner_size);
				}

		#pragma omp taskwait

		for (ii=kk+1; ii<size; ii++)
//          if (BENCH[ii*size+kk] != NULL)
			if((ii * 2 + kk * 3) % 10 == 1)
			for (jj=kk+1; jj<size; jj++)
//              if (BENCH[kk*size+jj] != NULL)
				if((kk * 2 + jj * 3) % 10 == 1)
				#pragma omp task untied firstprivate(kk, jj, ii) shared(BENCH)
				{
				//                     if (BENCH[ii*size+jj]==NULL) BENCH[ii*size+jj] = allocate_clean_block();
					bmod<AT>(BENCH[ii*size+kk], BENCH[kk*size+jj], BENCH[ii*size+jj], inner_size);
				}

		#pragma omp taskwait
	}
}

template <typename T, typename AT, typename AT2>
void Kernel(AT2 matrix, size_t size, size_t inner_size)
{
	LOC_PAPI_BEGIN_BLOCK

	sparselu_par_call<AT, AT2>(matrix, size, inner_size);
	
	LOC_PAPI_END_BLOCK
}

#endif
