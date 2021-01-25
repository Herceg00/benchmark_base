#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

#include "qubit_evolution.h"

typedef std::complex<double> base_type;
typedef base_type vec_type[1 << N];

typedef base_type matrix_type[U_MAT_DIM][U_MAT_DIM];

double CallKernel()
{
	static vec_type in;
	static vec_type out;
	static matrix_type u;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<vec_type, matrix_type>(in, out, u, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel1<vec_type, matrix_type>(in, out, u, N, N / 2);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<vec_type>(out, N));

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
