#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> vec_type;
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "jacobi.h"

double CallKernel()
{
	static matrix_type matrix("matrix", DIM2(N, N));
	static vec_type f("f", DIM1(N));
	static vec_type x("x", DIM1(N));
	static vec_type temp_x("temp_x", DIM1(N));

	timeval start, end;

	Init<base_type, vec_type, matrix_type>(matrix, f, x, temp_x, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, vec_type, matrix_type>(matrix, f, x, temp_x, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, vec_type>(x, N));

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
