#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
#define WEIRD_CONST 128

typedef uint64_t base_type;
typedef base_type array_type[N];
typedef base_type helper_type[WEIRD_CONST];

#include "rand.h"

double CallKernel()
{
	static array_type table;
	static helper_type ran;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lu\n"
			, Check<base_type, array_type, helper_type>(table, ran, N, WEIRD_CONST));

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

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
