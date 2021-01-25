#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

static const int N = SIZE_PREDEF;

typedef int base_type;
typedef base_type array_type[N];

#include "prime.h"

double CallKernel(int core_type)
{
	static array_type array;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(array, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		CallKernel<base_type, array_type>(core_type, array, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %d\n", Check<base_type, array_type>(array, N));

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
