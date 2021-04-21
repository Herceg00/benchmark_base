#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

static const int N = SIZE_PREDEF;

typedef double base_type;
typedef base_type matrix_type[N][N];

#include "holecky.h"

double CallKernel(int core_type)
{
	static matrix_type matrix;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, matrix_type>(matrix, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		CallKernel<base_type, matrix_type>(core_type, matrix, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, matrix_type>(matrix, N));

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

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}

	return 0;
}
