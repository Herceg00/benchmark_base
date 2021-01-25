#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef int base_type;
typedef locality::PtrWrapper<base_type, 1> array_type;

#include "prime.h"

double CallKernel(int core_type)
{
	static array_type array("array", DIM1(N));

	timeval start, end;

	Init<base_type, array_type>(array, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	CallKernel<base_type, array_type>(core_type, array, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %d\n", Check<base_type, array_type>(array, N));

	return locality::utils::TimeDif(start, end);
}

int main()
{
#ifdef LOC_LOG_PATH
	locality::memory_trace::Init(LOC_LOG_PATH);
#endif

#ifdef LOC_CVG_PARAMS
	locality::cvg::Init(LOC_CVG_PARAMS);
#endif

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif
	}

	return 0;
}
