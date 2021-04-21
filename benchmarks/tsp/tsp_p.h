#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef int base_type;
typedef locality::PtrWrapper<base_type, 2> paths_type;
typedef locality::PtrWrapper<base_type, 2> dist_type;
typedef locality::PtrWrapper<base_type, 1> tmp_type;

#include "tsp.h"

double CallKernel(int core_type)
{
	static paths_type paths("paths", DIM2(N, 1 << N));
	static dist_type dist("dist", DIM2(N + 1, N + 1));
	static tmp_type tmp("tmp", DIM1(N));

	base_type result;
	timeval start, end;

	Init<base_type, paths_type, dist_type, tmp_type>(paths, dist, tmp, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	CallKernel<base_type, paths_type, dist_type, tmp_type>(core_type, paths, dist, tmp, result, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %d\n", result);

	return locality::utils::TimeDif(start, end);
}

extern "C" int main()
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
