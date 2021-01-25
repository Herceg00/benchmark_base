#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[N];
typedef size_t helper_type[N];

#include "triada.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, N);

		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

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

	for(int core_type = 12; core_type < 1 + CORE_TYPES; core_type++)
	{
		locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}
}
