#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#include <iostream>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> array_type;

#include "fft_real.h"

double CallKernel()
{
	array_type data("data", DIM1(N * 2 + 2));
	array_type dOut("dOut", DIM1(N / 2));
	array_type dIn("dIn", DIM1(N));

	timeval start, end;

	Init<base_type, array_type>(dIn, dOut, data, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, array_type>(dIn, dOut, data, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, array_type>(dOut, N / 2));

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
	double time = CallKernel();



#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif

	return 0;
}
