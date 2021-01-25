#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <algorithm>

#include <sys/time.h>
#include <stdint.h>

namespace locality {

const uint64_t max_uint64_t = 18446744073709551615u;

class N128
{
private:
	uint64_t high, low;

public:
	void Add(uint64_t add)
	{
		if(add > max_uint64_t - low)
			high += 1;
		low += add; // overflw is ok here
	}

	double GetDiv(uint64_t count)
	{
		return (double) max_uint64_t / count * high + (double) low / count;
	}

	N128(int _low = 0):
		high(0),
		low(_low)
	{}
};

class utils
{
public:
	template<class T>
	static std::string ToString(T i)
	{
		std::stringstream tmp;
		tmp << i;

		return tmp.str();
	}

	static const long CACHE_ANNIL = 12*1024*1024; // size of array for cache spoiling

	static void CacheAnnil()
	{
		CacheAnnil(0);
	}
	
	static void CacheAnnil(int j)
	{
		int helper = 0;
		int *cache_annil = new int[CACHE_ANNIL];
		cache_annil[0] = 0;

		for (int i = 1; i < CACHE_ANNIL; i++)
		{
			cache_annil[i] = i-j;
			helper += cache_annil[i-1];
		}

		delete[] cache_annil;

		printf("cache_annil: %d\n", helper);
	}

	// return time dif in sec
	static double TimeDif(timeval start, timeval end)
	{
		return (end.tv_sec - start.tv_sec) + (end.tv_usec-start.tv_usec) / 1000000.0;
	}

	static double RRand(int i)
	{
		srand(i);
		return 1.0 * (rand() % RAND_MAX) / RAND_MAX;
	}

	static double RRand(int i, int j)
	{
		assert(sizeof(int) == 4); // o_O why is it here

		srand((i << 16) + j);
		return 1.0 * (rand() % RAND_MAX) / RAND_MAX;
	}

	static uint64_t GetThreadID()
	{
		return (uint64_t)pthread_self();
	}


/**
it is here (not in ptrwrapper), because ptrwrapper is a template class
*/
	static size_t& GlobalOffset()
	{
		static size_t global_offset = 0;

		return global_offset;
	}

	static size_t NextGlobalOffset(size_t new_size)
	{
		size_t& offset = GlobalOffset();

		offset += new_size;

		return offset - new_size;
	}

};
} /*namespace*/


#endif /*UTILS_H*/
