#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
#define WEIRD_CONST 128

typedef uint64_t base_type;
typedef locality::PtrWrapper<base_type, 1> array_type;
typedef locality::PtrWrapper<base_type, 1> helper_type;

#include "rand.h"

double CallKernel()
{
	static array_type table("table", DIM1(N));
	static helper_type ran("ran", DIM1(WEIRD_CONST));

	timeval start, end;

	Init<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST);

	locality::utils::CacheAnnil();

gettimeofday(&start, NULL);
locality::profiler::Logging(true);

		Kernel<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST);

locality::profiler::Logging(false);
gettimeofday(&end, NULL);

	printf("check_sum: %lu\n"
		, Check<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST));

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
