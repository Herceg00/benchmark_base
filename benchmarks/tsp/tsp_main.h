#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

static const int N = SIZE_PREDEF;

typedef int base_type;
typedef base_type paths_type[N][1 << N];
typedef base_type dist_type[N + 1][N + 1];
typedef base_type tmp_type[N];

#include "tsp.h"

double CallKernel(int core_type)
{
	static paths_type paths;
	static dist_type dist;
	static tmp_type tmp;

	base_type result;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, paths_type, dist_type, tmp_type>(paths, dist, tmp, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		CallKernel<base_type, paths_type, dist_type, tmp_type>(core_type, paths, dist, tmp, result, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %d\n", result);

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
