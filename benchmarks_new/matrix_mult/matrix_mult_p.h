#include <stdio.h>
#include <sys/time.h>

#include "locality.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 2> array_type;

#include "matrix_mult.h"

using namespace matrix_mult;

double CallKernel(int core_type)
{
	static array_type a("a", DIM2(N, N));
	static array_type b("b", DIM2(N, N));
	static array_type c("c", DIM2(N, N));

	timeval start, end;

	Init<base_type, array_type>(a, b, c, N);

	locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);
locality::profiler::Logging(true);

	CallKernel<base_type, array_type> (core_type, a, b, c, N);

locality::profiler::Logging(false);
gettimeofday(&end, NULL);

	printf("check_sum: %lg\n", Check<base_type, array_type>(c, N));

	double time = locality::utils::TimeDif(start, end);

	return time;
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
}

