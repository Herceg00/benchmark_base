#include <stdio.h>
#include <sys/time.h>

#include "locality.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef locality::PtrWrapper<base_type, 2> array_type;

#include "strassen.h"

class WrapperAllocator : public MatrixAllocator<array_type>
{
private:

public:
	array_type Allocate(size_t size)
	{
		static int counter = 0;

		return locality::PtrWrapper<base_type, 2>("array" + locality::utils::ToString(counter++), DIM2(size, size));
	}

	void Free(array_type, size_t) {}
};

double CallKernel(int core_type)
{
	static WrapperAllocator* allocator = new WrapperAllocator();

	static array_type a = allocator->Allocate(N);
	static array_type b = allocator->Allocate(N);
	static array_type c = allocator->Allocate(N);

	timeval start, end;

	Init<base_type, array_type>(a, b, c, N);

	locality::utils::CacheAnnil();

gettimeofday(&start, NULL);
locality::profiler::Logging(true);

	CallKernel<base_type, array_type> (core_type, a, b, c, N, allocator);

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

