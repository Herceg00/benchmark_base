#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 3> input_type;
typedef locality::PtrWrapper<base_type, 1> service_type;

#include "poisson.h"

double CallKernel()
{
	input_type input("input", DIM3(N, N, N * 2));
	service_type tmp_x("tmp_x", DIM1(N * 2));
	service_type tmp_y("tmp_y", DIM1(N * 2));
	service_type tmp_z("tmp_z", DIM1(N * 2));
	service_type pov_x("pov_x", DIM1(N * 2));
	service_type pov_y("pov_y", DIM1(N * 2));
	service_type pov_z("pov_z", DIM1(N * 2));

	timeval start, end;

	Init<base_type, service_type, input_type>(input
		, tmp_x, tmp_y, tmp_z
		, pov_x, pov_y, pov_z
		, N, N, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, service_type, input_type>(input
		, tmp_x, tmp_y, tmp_z
		, pov_x, pov_y, pov_z
		, N, N, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n"
		, Check<base_type, input_type>(input, N, N, N));

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
