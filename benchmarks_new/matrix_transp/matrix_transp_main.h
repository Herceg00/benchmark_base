#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>
#include "../../locutils_new/timers.h"


#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_transp.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;

	double time = -1;
    size_t bytes_requested = 2 * sizeof(double) * LENGTH * LENGTH;
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		CallKernel<base_type, array_type> (core_type, a, b, BLOCK_SIZE, LENGTH);

        counter.end_timing();

        counter.update_counters(bytes_requested, 0);

        counter.print_local_counters();

	}

    counter.print_average_counters(false);
    return time;
}

int main()
{
	LOC_PAPI_INIT

	for(int core_type = (int)MODE; core_type < (int)MODE + 1; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

locality::plain::Print(LENGTH, time);
	}
}

