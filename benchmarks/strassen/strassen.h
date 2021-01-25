#ifndef STRASSEN
#define STRASSEN

#include "matrix_mult.h"

const int BENCH_COUNT = 6;

const char* type_names[BENCH_COUNT] = {
	"normal", // 0
	"transform4", // 1
	"transform34", // 2
	"parallel_if_cutoff", // 3
	"parallel_manual_cutoff", // 4
	"parallel_no", // 5
};

template <typename T, typename AT>
void Init(AT a, AT b, AT c, size_t size)
{
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
		{
			a[i][j] = locality::utils::RRand(i, j);
			b[i][j] = locality::utils::RRand(j, i);
			c[i][j] = 0.0;
		}
}

template <typename T, typename AT>
T Check(AT c, size_t size)
{
	T s = 0.0;

	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
			s += c[i][j] / size / size;

	return s;
}

template <typename T>
class MatrixAllocator
{
private:

public:
	virtual T Allocate(size_t size) = 0;
	virtual void Free(T, size_t size) = 0;
};

template <typename AT>
void Sum(AT a, AT b, AT result, size_t size)
{
	for(size_t i = 0; i < size; i++) {
		for(size_t j = 0; j < size; j++) {
			result[i][j] = a[i][j] + b[i][j];
		}
	}
}

template <typename AT>
void Subtract(AT a, AT b, AT result, size_t size)
{
	for(size_t i = 0; i < size; i++) {
		for(size_t j = 0; j < size; j++) {
			result[i][j] = a[i][j] - b[i][j];
		}
	}
}

//https://en.wikipedia.org/w/index.php?title=Strassen_algorithm&oldid=498910018#Source_code_of_the_Strassen_algorithm_in_C_language

/*** NORMAL ***/
template <typename T, typename AT>
void NormalStrassen(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT aResult = allocator->Allocate(new_size);
		AT bResult = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];

				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];

				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				aResult[i][j] = 0;
				bResult[i][j] = 0;
			}
		}

		// Calculating p1 to p7:

		Sum(a11, a22, aResult, new_size); // a11 + a22
		Sum(b11, b22, bResult, new_size); // b11 + b22
		NormalStrassen<T, AT>(aResult, bResult, p1, new_size, allocator); // p1 = (a11+a22) * (b11+b22)

		Sum(a21, a22, aResult, new_size); // a21 + a22
		NormalStrassen<T, AT>(aResult, b11, p2, new_size, allocator); // p2 = (a21+a22) * (b11)

		Subtract(b12, b22, bResult, new_size); // b12 - b22
		NormalStrassen<T, AT>(a11, bResult, p3, new_size, allocator); // p3 = (a11) * (b12 - b22)

		Subtract(b21, b11, bResult, new_size); // b21 - b11
		NormalStrassen<T, AT>(a22, bResult, p4, new_size, allocator); // p4 = (a22) * (b21 - b11)

		Sum(a11, a12, aResult, new_size); // a11 + a12
		NormalStrassen<T, AT>(aResult, b22, p5, new_size, allocator); // p5 = (a11+a12) * (b22)

		Subtract(a21, a11, aResult, new_size); // a21 - a11
		Sum(b11, b12, bResult, new_size); // b11 + b12
		NormalStrassen<T, AT>(aResult, bResult, p6, new_size, allocator); // p6 = (a21-a11) * (b11+b12)

		Subtract(a12, a22, aResult, new_size); // a12 - a22
		Sum(b21, b22, bResult, new_size); // b21 + b22
		NormalStrassen<T, AT>(aResult, bResult, p7, new_size, allocator); // p7 = (a12-a22) * (b21+b22)

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, aResult, new_size); // p1 + p4
		Sum(aResult, p7, bResult, new_size); // p1 + p4 + p7
		Subtract(bResult, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, aResult, new_size); // p1 + p3
		Sum(aResult, p6, bResult, new_size); // p1 + p3 + p6
		Subtract(bResult, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(aResult, new_size);
		allocator->Free(bResult, new_size);

	} // end of else
} // end of Strassen function

static const int CUTOFF_VALUE = 4;

/*** PARALLEL ***/
template <typename T, typename AT>
void ParallelStrassen1(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator, int depth = 0)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT result1 = allocator->Allocate(new_size);
		AT result2 = allocator->Allocate(new_size);
		AT result3 = allocator->Allocate(new_size);
		AT result4 = allocator->Allocate(new_size);
		AT result5 = allocator->Allocate(new_size);
		AT result6 = allocator->Allocate(new_size);
		AT result7 = allocator->Allocate(new_size);
		AT result8 = allocator->Allocate(new_size);
		AT result9 = allocator->Allocate(new_size);
		AT result10 = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];

				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];

				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				result1[i][j] = 0;
				result2[i][j] = 0;
				result3[i][j] = 0;
				result4[i][j] = 0;
				result5[i][j] = 0;
				result6[i][j] = 0;
				result7[i][j] = 0;
				result8[i][j] = 0;
				result9[i][j] = 0;
				result10[i][j] = 0;
			}
		}

#pragma omp task untied if (depth < CUTOFF_VALUE)

		// Calculating p1 to p7:

		Sum(a11, a22, result1, new_size); // a11 + a22
		Sum(b11, b22, result2, new_size); // b11 + b22
		Sum(a21, a22, result3, new_size); // a21 + a22
		Subtract(b12, b22, result4, new_size); // b12 - b22
		Subtract(b21, b11, result5, new_size); // b21 - b11
		Sum(a11, a12, result6, new_size); // a11 + a12
		Subtract(a21, a11, result7, new_size); // a21 - a11
		Sum(b11, b12, result8, new_size); // b11 + b12
		Subtract(a12, a22, result9, new_size); // a12 - a22
		Sum(b21, b22, result10, new_size); // b21 + b22


#pragma omp task untied if (depth < CUTOFF_VALUE)
    		ParallelStrassen1<T, AT>(result1, result2, p1, new_size, allocator, depth + 1); // p1 = (a11+a22) * (b11+b22)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(result3, b11, p2, new_size, allocator, depth + 1); // p2 = (a21+a22) * (b11)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(a11, result4, p3, new_size, allocator, depth + 1); // p3 = (a11) * (b12 - b22)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(a22, result5, p4, new_size, allocator, depth + 1); // p4 = (a22) * (b21 - b11)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(result6, b22, p5, new_size, allocator, depth + 1); // p5 = (a11+a12) * (b22)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(result7, result8, p6, new_size, allocator, depth + 1); // p6 = (a21-a11) * (b11+b12)
#pragma omp task untied if (depth < CUTOFF_VALUE)
		ParallelStrassen1<T, AT>(result9, result10, p7, new_size, allocator, depth + 1); // p7 = (a12-a22) * (b21+b22)

#pragma omp taskwait

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, result1, new_size); // p1 + p4
		Sum(result1, p7, result2, new_size); // p1 + p4 + p7
		Subtract(result2, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, result1, new_size); // p1 + p3
		Sum(result1, p6, result2, new_size); // p1 + p3 + p6
		Subtract(result2, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(result1, new_size);
		allocator->Free(result2, new_size);
		allocator->Free(result3, new_size);
		allocator->Free(result4, new_size);
		allocator->Free(result5, new_size);
		allocator->Free(result6, new_size);
		allocator->Free(result7, new_size);
		allocator->Free(result8, new_size);
		allocator->Free(result9, new_size);
		allocator->Free(result10, new_size);
	} // end of else
} // end of Strassen function



/*** PARALLEL ***/
template <typename T, typename AT>
void ParallelStrassen2(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator, int depth = 0)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT result1 = allocator->Allocate(new_size);
		AT result2 = allocator->Allocate(new_size);
		AT result3 = allocator->Allocate(new_size);
		AT result4 = allocator->Allocate(new_size);
		AT result5 = allocator->Allocate(new_size);
		AT result6 = allocator->Allocate(new_size);
		AT result7 = allocator->Allocate(new_size);
		AT result8 = allocator->Allocate(new_size);
		AT result9 = allocator->Allocate(new_size);
		AT result10 = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];

				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];

				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				result1[i][j] = 0;
				result2[i][j] = 0;
				result3[i][j] = 0;
				result4[i][j] = 0;
				result5[i][j] = 0;
				result6[i][j] = 0;
				result7[i][j] = 0;
				result8[i][j] = 0;
				result9[i][j] = 0;
				result10[i][j] = 0;
			}
		}

#pragma omp task untied if (depth < CUTOFF_VALUE)

		// Calculating p1 to p7:

		Sum(a11, a22, result1, new_size); // a11 + a22
		Sum(b11, b22, result2, new_size); // b11 + b22
		Sum(a21, a22, result3, new_size); // a21 + a22
		Subtract(b12, b22, result4, new_size); // b12 - b22
		Subtract(b21, b11, result5, new_size); // b21 - b11
		Sum(a11, a12, result6, new_size); // a11 + a12
		Subtract(a21, a11, result7, new_size); // a21 - a11
		Sum(b11, b12, result8, new_size); // b11 + b12
		Subtract(a12, a22, result9, new_size); // a12 - a22
		Sum(b21, b22, result10, new_size); // b21 + b22

		if(depth < CUTOFF_VALUE)
		{
#pragma omp task untied
	    		ParallelStrassen2<T, AT>(result1, result2, p1, new_size, allocator, depth + 1); // p1 = (a11+a22) * (b11+b22)
#pragma omp task untied
			ParallelStrassen2<T, AT>(result3, b11, p2, new_size, allocator, depth + 1); // p2 = (a21+a22) * (b11)
#pragma omp task untied
			ParallelStrassen2<T, AT>(a11, result4, p3, new_size, allocator, depth + 1); // p3 = (a11) * (b12 - b22)
#pragma omp task untied
			ParallelStrassen2<T, AT>(a22, result5, p4, new_size, allocator, depth + 1); // p4 = (a22) * (b21 - b11)
#pragma omp task untied
			ParallelStrassen2<T, AT>(result6, b22, p5, new_size, allocator, depth + 1); // p5 = (a11+a12) * (b22)
#pragma omp task untied
			ParallelStrassen2<T, AT>(result7, result8, p6, new_size, allocator, depth + 1); // p6 = (a21-a11) * (b11+b12)
#pragma omp task untied
			ParallelStrassen2<T, AT>(result9, result10, p7, new_size, allocator, depth + 1); // p7 = (a12-a22) * (b21+b22)

#pragma omp taskwait
		}

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, result1, new_size); // p1 + p4
		Sum(result1, p7, result2, new_size); // p1 + p4 + p7
		Subtract(result2, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, result1, new_size); // p1 + p3
		Sum(result1, p6, result2, new_size); // p1 + p3 + p6
		Subtract(result2, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(result1, new_size);
		allocator->Free(result2, new_size);
		allocator->Free(result3, new_size);
		allocator->Free(result4, new_size);
		allocator->Free(result5, new_size);
		allocator->Free(result6, new_size);
		allocator->Free(result7, new_size);
		allocator->Free(result8, new_size);
		allocator->Free(result9, new_size);
		allocator->Free(result10, new_size);
	} // end of else
} // end of Strassen function

/*** PARALLEL ***/
template <typename T, typename AT>
void ParallelStrassen3(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator, int depth = 0)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT result1 = allocator->Allocate(new_size);
		AT result2 = allocator->Allocate(new_size);
		AT result3 = allocator->Allocate(new_size);
		AT result4 = allocator->Allocate(new_size);
		AT result5 = allocator->Allocate(new_size);
		AT result6 = allocator->Allocate(new_size);
		AT result7 = allocator->Allocate(new_size);
		AT result8 = allocator->Allocate(new_size);
		AT result9 = allocator->Allocate(new_size);
		AT result10 = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];

				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];

				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				result1[i][j] = 0;
				result2[i][j] = 0;
				result3[i][j] = 0;
				result4[i][j] = 0;
				result5[i][j] = 0;
				result6[i][j] = 0;
				result7[i][j] = 0;
				result8[i][j] = 0;
				result9[i][j] = 0;
				result10[i][j] = 0;
			}
		}

#pragma omp task untied if (depth < CUTOFF_VALUE)

		// Calculating p1 to p7:

		Sum(a11, a22, result1, new_size); // a11 + a22
		Sum(b11, b22, result2, new_size); // b11 + b22
		Sum(a21, a22, result3, new_size); // a21 + a22
		Subtract(b12, b22, result4, new_size); // b12 - b22
		Subtract(b21, b11, result5, new_size); // b21 - b11
		Sum(a11, a12, result6, new_size); // a11 + a12
		Subtract(a21, a11, result7, new_size); // a21 - a11
		Sum(b11, b12, result8, new_size); // b11 + b12
		Subtract(a12, a22, result9, new_size); // a12 - a22
		Sum(b21, b22, result10, new_size); // b21 + b22


#pragma omp task untied
    		ParallelStrassen3<T, AT>(result1, result2, p1, new_size, allocator, depth + 1); // p1 = (a11+a22) * (b11+b22)
#pragma omp task untied
		ParallelStrassen3<T, AT>(result3, b11, p2, new_size, allocator, depth + 1); // p2 = (a21+a22) * (b11)
#pragma omp task untied
		ParallelStrassen3<T, AT>(a11, result4, p3, new_size, allocator, depth + 1); // p3 = (a11) * (b12 - b22)
#pragma omp task untied
		ParallelStrassen3<T, AT>(a22, result5, p4, new_size, allocator, depth + 1); // p4 = (a22) * (b21 - b11)
#pragma omp task untied
		ParallelStrassen3<T, AT>(result6, b22, p5, new_size, allocator, depth + 1); // p5 = (a11+a12) * (b22)
#pragma omp task untied
		ParallelStrassen3<T, AT>(result7, result8, p6, new_size, allocator, depth + 1); // p6 = (a21-a11) * (b11+b12)
#pragma omp task untied
		ParallelStrassen3<T, AT>(result9, result10, p7, new_size, allocator, depth + 1); // p7 = (a12-a22) * (b21+b22)

#pragma omp taskwait

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, result1, new_size); // p1 + p4
		Sum(result1, p7, result2, new_size); // p1 + p4 + p7
		Subtract(result2, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, result1, new_size); // p1 + p3
		Sum(result1, p6, result2, new_size); // p1 + p3 + p6
		Subtract(result2, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(result1, new_size);
		allocator->Free(result2, new_size);
		allocator->Free(result3, new_size);
		allocator->Free(result4, new_size);
		allocator->Free(result5, new_size);
		allocator->Free(result6, new_size);
		allocator->Free(result7, new_size);
		allocator->Free(result8, new_size);
		allocator->Free(result9, new_size);
		allocator->Free(result10, new_size);
	} // end of else
} // end of Strassen function


/*** SPLITTED 1 NESTED LOOP IN THE BEGGINING INTO 3 ***/
template <typename T, typename AT>
void Transform4Strassen(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT aResult = allocator->Allocate(new_size);
		AT bResult = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];
			}
		}

		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];
			}
		}

		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				aResult[i][j] = 0;
				bResult[i][j] = 0;
			}
		}

		// Calculating p1 to p7:

		Sum(a11, a22, aResult, new_size); // a11 + a22
		Sum(b11, b22, bResult, new_size); // b11 + b22
		Transform4Strassen<T, AT>(aResult, bResult, p1, new_size, allocator); // p1 = (a11+a22) * (b11+b22)

		Sum(a21, a22, aResult, new_size); // a21 + a22
		Transform4Strassen<T, AT>(aResult, b11, p2, new_size, allocator); // p2 = (a21+a22) * (b11)

		Subtract(b12, b22, bResult, new_size); // b12 - b22
		Transform4Strassen<T, AT>(a11, bResult, p3, new_size, allocator); // p3 = (a11) * (b12 - b22)

		Subtract(b21, b11, bResult, new_size); // b21 - b11
		Transform4Strassen<T, AT>(a22, bResult, p4, new_size, allocator); // p4 = (a22) * (b21 - b11)

		Sum(a11, a12, aResult, new_size); // a11 + a12
		Transform4Strassen<T, AT>(aResult, b22, p5, new_size, allocator); // p5 = (a11+a12) * (b22)

		Subtract(a21, a11, aResult, new_size); // a21 - a11
		Sum(b11, b12, bResult, new_size); // b11 + b12
		Transform4Strassen<T, AT>(aResult, bResult, p6, new_size, allocator); // p6 = (a21-a11) * (b11+b12)

		Subtract(a12, a22, aResult, new_size); // a12 - a22
		Sum(b21, b22, bResult, new_size); // b21 + b22
		Transform4Strassen<T, AT>(aResult, bResult, p7, new_size, allocator); // p7 = (a12-a22) * (b21+b22)

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, aResult, new_size); // p1 + p4
		Sum(aResult, p7, bResult, new_size); // p1 + p4 + p7
		Subtract(bResult, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, aResult, new_size); // p1 + p3
		Sum(aResult, p6, bResult, new_size); // p1 + p3 + p6
		Subtract(bResult, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(aResult, new_size);
		allocator->Free(bResult, new_size);

	} // end of else
} // end of Strassen function



/*** SWITCHED ORDER OF RECURSIONS ***/
template <typename T, typename AT>
void Transform3Strassen(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT aResult = allocator->Allocate(new_size);
		AT bResult = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];

				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];

				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				aResult[i][j] = 0;
				bResult[i][j] = 0;
			}
		}

		// Calculating p1 to p7:

		Sum(a11, a22, aResult, new_size); // a11 + a22
		Sum(b11, b22, bResult, new_size); // b11 + b22
		Transform3Strassen<T, AT>(aResult, bResult, p1, new_size, allocator); // p1 = (a11+a22) * (b11+b22)

		Subtract(b12, b22, bResult, new_size); // b12 - b22
		Transform3Strassen<T, AT>(a11, bResult, p3, new_size, allocator); // p3 = (a11) * (b12 - b22)

		Sum(a11, a12, aResult, new_size); // a11 + a12
		Transform3Strassen<T, AT>(aResult, b22, p5, new_size, allocator); // p5 = (a11+a12) * (b22)

		Subtract(a21, a11, aResult, new_size); // a21 - a11
		Sum(b11, b12, bResult, new_size); // b11 + b12
		Transform3Strassen<T, AT>(aResult, bResult, p6, new_size, allocator); // p6 = (a21-a11) * (b11+b12)

		Sum(a21, a22, aResult, new_size); // a21 + a22
		Transform3Strassen<T, AT>(aResult, b11, p2, new_size, allocator); // p2 = (a21+a22) * (b11)

		Subtract(b21, b11, bResult, new_size); // b21 - b11
		Transform3Strassen<T, AT>(a22, bResult, p4, new_size, allocator); // p4 = (a22) * (b21 - b11)

		Subtract(a12, a22, aResult, new_size); // a12 - a22
		Sum(b21, b22, bResult, new_size); // b21 + b22
		Transform3Strassen<T, AT>(aResult, bResult, p7, new_size, allocator); // p7 = (a12-a22) * (b21+b22)

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, aResult, new_size); // p1 + p4
		Sum(aResult, p7, bResult, new_size); // p1 + p4 + p7
		Subtract(bResult, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, aResult, new_size); // p1 + p3
		Sum(aResult, p6, bResult, new_size); // p1 + p3 + p6
		Subtract(bResult, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(aResult, new_size);
		allocator->Free(bResult, new_size);

	} // end of else
} // end of Strassen function


/*** SWITCHED ORDER OF RECURSIONS AND SPLITTED LOOP ***/
template <typename T, typename AT>
void Transform34Strassen(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	// trivial case: when the matrix is 1 X 1:
	if(size <= 16)
	{
		matrix_mult::KernelIKJ<T, AT>(a, b, c, size);
		return;
	}

	// other cases are treated here:
	else
	{
		size_t new_size = size / 2;

		// memory allocation:
		AT a11 = allocator->Allocate(new_size);
		AT a12 = allocator->Allocate(new_size);
		AT a21 = allocator->Allocate(new_size);
		AT a22 = allocator->Allocate(new_size);

		AT b11 = allocator->Allocate(new_size);
		AT b12 = allocator->Allocate(new_size);
		AT b21 = allocator->Allocate(new_size);
		AT b22 = allocator->Allocate(new_size);

		AT c11 = allocator->Allocate(new_size);
		AT c12 = allocator->Allocate(new_size);
		AT c21 = allocator->Allocate(new_size);
		AT c22 = allocator->Allocate(new_size);

		AT p1 = allocator->Allocate(new_size);
		AT p2 = allocator->Allocate(new_size);
		AT p3 = allocator->Allocate(new_size);
		AT p4 = allocator->Allocate(new_size);
		AT p5 = allocator->Allocate(new_size);
		AT p6 = allocator->Allocate(new_size);
		AT p7 = allocator->Allocate(new_size);

		AT aResult = allocator->Allocate(new_size);
		AT bResult = allocator->Allocate(new_size);

		//dividing the matrices in 4 sub-matrices:
		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				a11[i][j] = a[i][j];
				a12[i][j] = a[i][j + new_size];
				a21[i][j] = a[i + new_size][j];
				a22[i][j] = a[i + new_size][j + new_size];
			}
		}

		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				b11[i][j] = b[i][j];
				b12[i][j] = b[i][j + new_size];
				b21[i][j] = b[i + new_size][j];
				b22[i][j] = b[i + new_size][j + new_size];
			}
		}

		for(size_t i = 0; i < new_size; i++)
		{
			for(size_t j = 0; j < new_size; j++)
			{
				c11[i][j] = 0;
				c12[i][j] = 0;
				c21[i][j] = 0;
				c22[i][j] = 0;

				p1[i][j] = 0;
				p2[i][j] = 0;
				p3[i][j] = 0;
				p4[i][j] = 0;
				p5[i][j] = 0;
				p6[i][j] = 0;
				p7[i][j] = 0;

				aResult[i][j] = 0;
				bResult[i][j] = 0;
			}
		}

		// Calculating p1 to p7:

		Sum(a11, a22, aResult, new_size); // a11 + a22
		Sum(b11, b22, bResult, new_size); // b11 + b22
		Transform34Strassen<T, AT>(aResult, bResult, p1, new_size, allocator); // p1 = (a11+a22) * (b11+b22)

		Subtract(b12, b22, bResult, new_size); // b12 - b22
		Transform34Strassen<T, AT>(a11, bResult, p3, new_size, allocator); // p3 = (a11) * (b12 - b22)

		Sum(a11, a12, aResult, new_size); // a11 + a12
		Transform34Strassen<T, AT>(aResult, b22, p5, new_size, allocator); // p5 = (a11+a12) * (b22)

		Subtract(a21, a11, aResult, new_size); // a21 - a11
		Sum(b11, b12, bResult, new_size); // b11 + b12
		Transform34Strassen<T, AT>(aResult, bResult, p6, new_size, allocator); // p6 = (a21-a11) * (b11+b12)

		Sum(a21, a22, aResult, new_size); // a21 + a22
		Transform34Strassen<T, AT>(aResult, b11, p2, new_size, allocator); // p2 = (a21+a22) * (b11)

		Subtract(b21, b11, bResult, new_size); // b21 - b11
		Transform34Strassen<T, AT>(a22, bResult, p4, new_size, allocator); // p4 = (a22) * (b21 - b11)

		Subtract(a12, a22, aResult, new_size); // a12 - a22
		Sum(b21, b22, bResult, new_size); // b21 + b22
		Transform34Strassen<T, AT>(aResult, bResult, p7, new_size, allocator); // p7 = (a12-a22) * (b21+b22)

		// calculating c21, c21, c11 e c22:

		Sum(p3, p5, c12, new_size); // c12 = p3 + p5
		Sum(p2, p4, c21, new_size); // c21 = p2 + p4

		Sum(p1, p4, aResult, new_size); // p1 + p4
		Sum(aResult, p7, bResult, new_size); // p1 + p4 + p7
		Subtract(bResult, p5, c11, new_size); // c11 = p1 + p4 - p5 + p7

		Sum(p1, p3, aResult, new_size); // p1 + p3
		Sum(aResult, p6, bResult, new_size); // p1 + p3 + p6
		Subtract(bResult, p2, c22, new_size); // c22 = p1 + p3 - p2 + p6

		// Grouping the results obtained in a single matrix:
		for(size_t i = 0; i < new_size ; i++)
		{
			for(size_t j = 0 ; j < new_size ; j++)
			{
				c[i][j] = c11[i][j];
				c[i][j + new_size] = c12[i][j];
				c[i + new_size][j] = c21[i][j];
				c[i + new_size][j + new_size] = c22[i][j];
			}
		}

		allocator->Free(a11, new_size);
		allocator->Free(a12, new_size);
		allocator->Free(a21, new_size);
		allocator->Free(a22, new_size);

		allocator->Free(b11, new_size);
		allocator->Free(b12, new_size);
		allocator->Free(b21, new_size);
		allocator->Free(b22, new_size);

		allocator->Free(c11, new_size);
		allocator->Free(c12, new_size);
		allocator->Free(c21, new_size);
		allocator->Free(c22, new_size);

		allocator->Free(p1, new_size);
		allocator->Free(p2, new_size);
		allocator->Free(p3, new_size);
		allocator->Free(p4, new_size);
		allocator->Free(p5, new_size);
		allocator->Free(p6, new_size);
		allocator->Free(p7, new_size);

		allocator->Free(aResult, new_size);
		allocator->Free(bResult, new_size);

	} // end of else
} // end of Strassen function



template <typename T, typename AT>
void NormalKernel(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	NormalStrassen<T, AT>(a, b, c, size, allocator);
}

template <typename T, typename AT>
void Transform4Kernel(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	Transform4Strassen<T, AT>(a, b, c, size, allocator);
}

template <typename T, typename AT>
void Transform3Kernel(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	Transform3Strassen<T, AT>(a, b, c, size, allocator);
}

template <typename T, typename AT>
void Transform34Kernel(AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	Transform34Strassen<T, AT>(a, b, c, size, allocator);
}

template <typename T, typename AT>
void CallKernel(int core_type, AT a, AT b, AT c, size_t size, MatrixAllocator<AT>* allocator)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		case 0: NormalKernel<T, AT>(a, b, c, size, allocator); break;

		case 1: Transform4Kernel<T, AT>(a, b, c, size, allocator); break;

		case 2: Transform34Kernel<T, AT>(a, b, c, size, allocator); break;

		case 3: 
#pragma omp parallel num_threads(LOC_THREADS)
#pragma omp single
#pragma omp task untied   
ParallelStrassen1<T, AT>(a, b, c, size, allocator); break;

		case 4: 
#pragma omp parallel num_threads(LOC_THREADS)
#pragma omp single
#pragma omp task untied   
ParallelStrassen2<T, AT>(a, b, c, size, allocator); break;

		case 5:

#pragma omp parallel num_threads(LOC_THREADS)
#pragma omp single
#pragma omp task untied   
ParallelStrassen3<T, AT>(a, b, c, size, allocator); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

#endif /*STRASSEN*/
