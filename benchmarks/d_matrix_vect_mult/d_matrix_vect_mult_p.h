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

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> vec_type;
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "d_matrix_vect_mult.h"

double CallKernel()
{
	matrix_type matrix("matrix" , DIM2(N, N));
	vec_type vec_in   ("vec_in" , DIM1(N));
	vec_type vec_out  ("vec_out", DIM1(N));

	timeval start, end;

	Init<base_type, vec_type, matrix_type>(vec_in, vec_out, matrix, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, vec_type, matrix_type>(vec_in, vec_out, matrix, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, vec_type>(vec_out, N));

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