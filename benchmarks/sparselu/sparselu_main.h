#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

static const int N = SIZE_PREDEF;

typedef double base_type;
typedef base_type* inner_type;
typedef base_type** matrix_type;

#include "sparselu.h"

double CallKernel()
{
	matrix_type matrix = new base_type*[N*N];

	for(size_t i = 0; i < N*N; i++)
		matrix[i] = new base_type[N];

	timeval start, end;
	double time = -1;

	int inner_size = std::sqrt(N);

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, inner_type, matrix_type>(matrix, N, inner_size);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, inner_type, matrix_type>(matrix, N, inner_size);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, inner_type, matrix_type>(matrix, N, inner_size));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

extern "C" int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
