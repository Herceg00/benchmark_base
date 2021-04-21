#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

#include "qubit_evolution.h"

typedef std::complex<double> base_type;
typedef locality::PtrWrapper<base_type, 1> vec_type;
typedef locality::PtrWrapper<base_type, 2> matrix_type;

double CallKernel()
{
	static vec_type in("in", DIM1(1 << N));
	static vec_type out("out", DIM1(1 << N));
	static matrix_type u("u", DIM2(2, 2));

	timeval start, end;

	Init<vec_type, matrix_type>(in, out, u, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel1<vec_type, matrix_type>(in, out, u, N, N / 2);

locality::profiler::Logging(false);
gettimeofday(&end, NULL);

	printf("check_sum: %lg\n", Check<vec_type>(out, N));

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
