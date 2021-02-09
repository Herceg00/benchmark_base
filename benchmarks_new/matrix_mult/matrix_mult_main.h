#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>


typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_mult.h"
#include "../../locutils_new/timers.h"

using namespace matrix_mult;


double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
    int bw_for_stat[6] = {(int)LENGTH * (int)LENGTH* (int)LENGTH * 2  + 2 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 2  + 2 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH};


	double time = -1;
    size_t bytes_requested = sizeof(double) * bw_for_stat[core_type];
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, c, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		CallKernel<base_type, array_type> (core_type, a, b, c, LENGTH);

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

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}

