#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef unsigned char base_type;
typedef base_type matrix_type[N][N];

#include "dithering.h"

double CallKernel(int core_type)
{
	static matrix_type src;
	static matrix_type dst;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, matrix_type>(src, dst, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		CallKernel<base_type, matrix_type>(core_type, src, dst, N, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type>(dst, N));

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

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}

	return 0;
}