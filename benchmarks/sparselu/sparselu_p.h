#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> inner_type;
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "sparselu.h"

double CallKernel()
{
	matrix_type matrix("matrix", DIM2(N * N, N));

	timeval start, end;
	int inner_size = std::sqrt(N);

	Init<base_type, inner_type, matrix_type>(matrix, N, inner_size);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, inner_type, matrix_type>(matrix, N, inner_size);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, inner_type, matrix_type>(matrix, N, inner_size));

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
