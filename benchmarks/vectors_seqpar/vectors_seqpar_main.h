#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[N];

#include "vectors_seqpar.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, c, N);
		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);

		CallKernel<base_type, array_type> (core_type, a, b, c, BLOCK_SIZE, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(c, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

int main()
{
	LOC_PAPI_INIT

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}
}

