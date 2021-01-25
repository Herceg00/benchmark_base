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

#include "fft_complex.h"

using namespace fft_complex;

double CallKernel()
{
	array_type x("x", DIM1(N * 2));
	array_type pov("pov", DIM1(N * 2 / 2));

	timeval start, end;

	Init<base_type, array_type>(x, pov, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, array_type>(x, pov, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, array_type>(x, N));

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

	double time = CallKernel();

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif

	return 0;
}
