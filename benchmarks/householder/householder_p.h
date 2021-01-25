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
typedef locality::PtrWrapper<base_type, 2> matrix_type;

#include "householder.h"

double CallKernel(int core_type)
{
	matrix_type a("a", DIM2(N, N));
	vector_type b("b", DIM1(N));
	vector_type aux("aux", DIM1(N));
	vector_type u("u", DIM1(N));

	timeval start, end;

	Init<base_type, vector_type, matrix_type>(a, b, aux, u, N, N);

	locality::utils::CacheAnnil(core_type);

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	CallKernel<base_type, vector_type, matrix_type>(core_type, a, b, aux, u, N, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, matrix_type>(a, N, N));

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
