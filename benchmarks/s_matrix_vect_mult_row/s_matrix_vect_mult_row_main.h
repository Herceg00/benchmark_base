#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#define UNSAFE_MODE

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
static const int SPARSITY = 10;

typedef double base_type;
typedef base_type* data_arr;

typedef size_t index_type;
typedef index_type* index_arr;

#include "s_matrix_vect_mult_row.h"

double CallKernel()
{
	SparseRowMatrixSpec spec = GenerateSpec(N, SPARSITY);

	SparseRowMatrix<data_arr, index_arr> matrix;

	matrix.a = new base_type[spec.a];
	matrix.b = new base_type[spec.b];
	matrix.x = new base_type[spec.x];
	matrix.ip = new index_type[spec.ip];
	matrix.ia = new index_type[spec.ia];

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		GenerateMatrix(matrix, spec);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, index_type, data_arr, index_arr>(matrix, spec);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, index_type, data_arr, index_arr>(matrix, spec));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	delete[] matrix.a;
	delete[] matrix.x;
	delete[] matrix.ip;
	delete[] matrix.ia;

	delete[] spec.lengths;

	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}