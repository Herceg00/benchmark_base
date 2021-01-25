#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef int indx_type[N];
typedef base_type vec_type[N];
typedef base_type matrix_type[N][N];

#include "gauss.h"

double CallKernel()
{
	static matrix_type matrix;
	static vec_type b;
	static vec_type x;
	static indx_type indx;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, vec_type>(x, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
