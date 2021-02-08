#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> array_type;
typedef locality::PtrWrapper<size_t, 1> helper_type;

#include "triada.h"

double CallKernel(int core_type)
{
	static array_type a("a", DIM1(N));
	static array_type b("b", DIM1(N));
	static array_type c("c", DIM1(N));
	static array_type x("x", DIM1(N));
	static helper_type ind("ind", DIM1(N));

	timeval start, end;

	Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, N);

	locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

	locality::utils::CacheAnnil(core_type);

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, N);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %lg\n", Check<base_type, array_type>(a, N));

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

	for(int core_type = 1; core_type < 1 + CORE_TYPES; core_type++)
	{
		locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

		double time = CallKernel(core_type);

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif
	}
}
