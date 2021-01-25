#ifndef HOUSEHOLDER_H
#define HOUSEHOLDER_H

#include <cmath>

const int BENCH_COUNT = 3;

const char* type_names[BENCH_COUNT] = {
	"normal", // 0
	"transform14", //1
	"transform1", //2
};

template <typename T, typename AT, typename AT2>
void Init(AT2 a, AT b, AT aux, AT u, size_t ncol, size_t nrow)
{
	for(size_t i = 0; i < ncol; i++)
		for(size_t j = 0; j < nrow; j++)
			a[i][j] = locality::utils::RRand(i, j);

	for(size_t i = 0; i < nrow; i++)
	{
		u[i] = locality::utils::RRand(i);
		b[i] = locality::utils::RRand(i);
	}

	for(size_t i = 0; i < ncol; i++)
		aux[i] = locality::utils::RRand(i);

}

template <typename T, typename AT2>
T Check(AT2 a, size_t ncol, size_t nrow)
{
	T sum = 0;

	for(size_t i = 0; i < ncol; i++)
		for(size_t j = 0; j < nrow; j++)
			sum += a[i][j];

	return sum;
}

#define sgn(a) ((a) == 0 ? 0 : ((a) > 0  ? 1 : -1 ))

// http://www.math.umd.edu/~mariakc/teaching-2/
/**** NORMAL ****/
template <typename T, typename AT, typename AT2>
void NormalKernel(AT2 a, AT b, AT aux, AT u, size_t ncol, size_t nrow)
{
	double normx, aaux, baux;

	/* Start the QR algorithm via Householder reflections */
	for(size_t i = 0; i < ncol; i++)
	{
		/*form vector u=House(a(i:nrow,i))*/
		normx = 0.0;
		for(size_t m = 0; m < nrow - i; m++)
		{
			aaux = a[i+m][i];  /* aaux = a_{m+i,i} */
			normx += aaux * aaux;  /* normx = normx + aaux*aaux */
		}

		aaux = a[i][i];    /* aaux = a_{i,i} */
		u[0] = aaux + sgn(aaux) * sqrt(normx);

#pragma omp parallel for num_threads(LOC_THREADS)
		for(size_t m = 1; m < nrow - i; m++)
			u[m] = a[i+m][i];  /* u[m] = a_{i+m,i} */

		normx = 0.0;

		for(size_t m = 0; m < nrow - i; m++)
			normx += u[m] * u[m];   /* normx = normx + u[m]*u[m] */

		/* compute (I-2uu^t)a(i:nrow,i:ncol) */

#pragma omp parallel for num_threads(LOC_THREADS)
		for(int l = 0; l < (int)(ncol - i); l++)
		{
			aux[l] = 0.0;
			for(size_t m = 0; m < nrow - i; m++)
			{
				aux[l] += u[m]*a[i+m][i+l];  /* aux[l] = aux[l] + u[m]*a_{i+m,i+l} */
			}

			aux[l] *= -2.0 / normx;     /* aux[l] = -2*aux[l]/normx */
			for(size_t m = 0; m < nrow - i; m++)
			{
				a[i+m][i+l] += u[m] * aux[l];	  /* a_{i+m,i+l} = a_{i+m,i+l} + u[m]*aux[l] */
			}
		}

		/* compute (Q^T)*b */
		baux = 0.0;
		for(size_t m = 0; m < nrow - i; m++)
			baux += u[m] * b[i+m];  /* baux = baux + u[m]*b[i+m] */

		baux *= -2.0 / normx;

		for(size_t m = 0; m < nrow - i; m++)
			b[i+m] += baux * u[m];  /* b[i+m] = b[i+m] + baux*u[m] */
	}
}

/**** IN MAIN LOOP 1) SWITCHED COMPUTATION OF aux AND baux; AND 2) COMBINED 3 LOOPS IN 1 ****/
template <typename T, typename AT, typename AT2>
void Transform1Kernel(AT2 a, AT b, AT aux, AT u, size_t ncol, size_t nrow)
{
		double normx, aaux, baux;

		/* Start the QR algorithm via Householder reflections */
		for(size_t i = 0; i < ncol; i++)
		{
			/*form vector u=House(a(i:nrow,i))*/
			normx = 0.0;
			for(size_t m = 0; m < nrow - i; m++)
			{
					aaux = a[i+m][i];  /* aaux = a_{m+i,i} */
					normx += aaux * aaux;  /* normx = normx + aaux*aaux */
			}

			aaux = a[i][i];    /* aaux = a_{i,i} */
			u[0] = aaux + sgn(aaux) * sqrt(normx);
			normx = u[0] * u[0];
			baux = u[0] * b[i];

			for(size_t m = 1; m < nrow - i; m++)
			{
				u[m] = a[i+m][i];  /* u[m] = a_{i+m,i} */
				normx += u[m] * u[m];   /* normx = normx + u[m]*u[m] */
				baux += u[m] * b[i+m];  /* baux = baux + u[m]*b[i+m] */
			}

			baux *= -2.0 / normx;

#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t m = 0; m < nrow - i; m++)
					b[i+m] += baux * u[m];  /* b[i+m] = b[i+m] + baux*u[m] */

			/* compute (I-2uu^t)a(i:nrow,i:ncol) */
#pragma omp parallel for num_threads(LOC_THREADS)
			for(int l = 0; l < (int)(ncol - i); l++)
			{
					aux[l] = 0.0;
					for(size_t m = 0; m < nrow - i; m++)
					{
							aux[l] += u[m]*a[i+m][i+l];  /* aux[l] = aux[l] + u[m]*a_{i+m,i+l} */
					}

					aux[l] *= -2.0 / normx;     /* aux[l] = -2*aux[l]/normx */
					for(size_t m = 0; m < nrow - i; m++)
					{
							a[i+m][i+l] += u[m] * aux[l];     /* a_{i+m,i+l} = a_{i+m,i+l} + u[m]*aux[l] */
					}
			}
		}
}

/**** LOOP of aux COMPUTATION SPLITTED INTO SEVERAL (AND LOOP NESTING ORDER CHANGED) ****/
template <typename T, typename AT, typename AT2>
void Transform14Kernel(AT2 a, AT b, AT aux, AT u, size_t ncol, size_t nrow)
{
		double normx, aaux, baux;

		/* Start the QR algorithm via Householder reflections */
		for(size_t i = 0; i < ncol; i++)
		{
			/*form vector u=House(a(i:nrow,i))*/
			normx = 0.0;
			for(size_t m = 0; m < nrow - i; m++)
			{
					aaux = a[i+m][i];  /* aaux = a_{m+i,i} */
					normx += aaux * aaux;  /* normx = normx + aaux*aaux */
			}

			aaux = a[i][i];    /* aaux = a_{i,i} */
			u[0] = aaux + sgn(aaux) * sqrt(normx);

#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t m = 1; m < nrow - i; m++)
					u[m] = a[i+m][i];  /* u[m] = a_{i+m,i} */

			normx = 0.0;

			for(size_t m = 0; m < nrow - i; m++)
					normx += u[m] * u[m];   /* normx = normx + u[m]*u[m] */

			/* compute (I-2uu^t)a(i:nrow,i:ncol) */

	/// SPLITTED INTO SEPARATE LOOPS
#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t l = 0; l < ncol - i; l++)
				aux[l] = 0.0;

#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t m = 0; m < nrow - i; m++)
			{
				for(size_t l = 0; l < ncol - i; l++)
				{
					aux[l] += u[m]*a[i+m][i+l];  /* aux[l] = aux[l] + u[m]*a_{i+m,i+l} */
				}
			}

#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t l = 0; l < ncol - i; l++)
				aux[l] *= -2.0 / normx; /* aux[l] = -2*aux[l]/normx */

#pragma omp parallel for num_threads(LOC_THREADS)
			for(size_t m = 0; m < nrow - i; m++)
			{
				for(size_t l = 0; l < ncol - i; l++)
				{
					a[i+m][i+l] += u[m] * aux[l];     /* a_{i+m,i+l} = a_{i+m,i+l} + u[m]*aux[l] */
				}
			}
	/// END ///

			/* compute (Q^T)*b */
			baux = 0.0;
			for(size_t m = 0; m < nrow - i; m++)
				baux += u[m] * b[i+m];  /* baux = baux + u[m]*b[i+m] */

			baux *= -2.0 / normx;

			for(size_t m = 0; m < nrow - i; m++)
				b[i+m] += baux * u[m];  /* b[i+m] = b[i+m] + baux*u[m] */
		}
}

template <typename T, typename AT, typename AT2>
void CallKernel(size_t core_type, AT2 a, AT b, AT aux, AT u, size_t ncol, size_t nrow)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		case 0: NormalKernel<T, AT, AT2>(a, b, aux, u, ncol, nrow); break;

		case 1: Transform14Kernel<T, AT, AT2>(a, b, aux, u, ncol, nrow); break;

		case 2: Transform1Kernel<T, AT, AT2>(a, b, aux, u, ncol, nrow); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

#endif /*HOUSEHOLDER_H*/
