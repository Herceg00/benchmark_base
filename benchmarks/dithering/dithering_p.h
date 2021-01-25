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
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "dithering.h"

double CallKernel(int core_type)
{
	matrix_type src("src" , DIM2(N, N));
	matrix_type dst("dst" , DIM2(N, N));

	timeval start, end;

	Init<base_type, matrix_type>(src, dst, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	CallKernel<base_type, matrix_type>(core_type, src, dst, N, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type>(dst, N));

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