#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<int, 1> indx_type;
typedef locality::PtrWrapper<base_type, 1> vec_type;
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "gauss_back.h"

double CallKernel()
{
	matrix_type matrix("matrix", DIM2(N, N));
	vec_type b("b", DIM1(N));
	vec_type x("x", DIM1(N));
	indx_type indx("indx", DIM1(N));

	timeval start, end;

	Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, N);

	locality::utils::CacheAnnil();

	Kernel1<base_type, matrix_type, indx_type>(matrix, indx, N);

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel2<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, vec_type>(x, N));

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
