#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> vector_type;

#include "cyclic_reduction.h"

double CallKernel()
{
	vector_type u_diag("u_diag", DIM1(N));
	vector_type diag("diag", DIM1(N));
	vector_type l_diag("l_diag", DIM1(N));
	vector_type right("right", DIM1(N));
	vector_type result("result", DIM1(N));

	timeval start, end;

	Init<base_type, vector_type>(u_diag, diag, l_diag, right, result, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, vector_type>(u_diag, diag, l_diag, right, result, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("                  check_sum: %lg\n", Check<base_type, vector_type>(result, N));

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
