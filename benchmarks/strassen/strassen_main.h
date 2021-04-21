#include <stdio.h>
#include <sys/time.h>

#include <vector>
#include <map>
#include <iostream>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type (**array_type);

#include "strassen.h"

class SimpleAllocator : public MatrixAllocator<array_type>
{
private:
	std::map<size_t, std::vector<array_type> > cache;

	bool Present(size_t size)
	{
		if(cache.find(size) == cache.end())
			return false;

		return !cache[size].empty();
	}

	array_type FromCache(size_t size)
	{
		array_type result = cache[size].back();
		cache[size].pop_back();

//		std::cout << "from cache: " << size << " " << result << std::endl;
		return result;
	}

public:
	array_type Allocate(size_t size)
	{
		array_type result;

#pragma omp critical
{
		if(Present(size))
			result = FromCache(size);

		else
		{
			result = new base_type*[size];

			for(size_t i = 0; i < size; i++)
				result[i] = new base_type[size];
		}
}
		return result;
	}

	void Free(array_type ptr, size_t size)
	{
#pragma omp critical
{
		if(cache.find(size) == cache.end())
			cache[size] = std::vector<array_type>();

		cache[size].push_back(ptr);
}
	}
};

double CallKernel(int core_type)
{
	static SimpleAllocator* allocator = new SimpleAllocator();

	static array_type a = allocator->Allocate(N);
	static array_type b = allocator->Allocate(N);
	static array_type c = allocator->Allocate(N);

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, c, N);
		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);

		CallKernel<base_type, array_type>(core_type, a, b, c, N, allocator);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(c, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

extern "C" int main()
{
	LOC_PAPI_INIT

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);
		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}
}
