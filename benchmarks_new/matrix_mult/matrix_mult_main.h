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


void CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;

#ifndef METRIC_RUN
    double bw_for_stat[6] = {(double)LENGTH * (double)LENGTH* (double)LENGTH * 2  + 2 * (double)LENGTH * (double)LENGTH,
                             (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                             (double)LENGTH * (double)LENGTH* (double)LENGTH * 2  + 2 * (double)LENGTH * (double)LENGTH,
                             (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                             (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                             (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH};

    size_t bytes_requested = sizeof(double) * bw_for_stat[core_type];
    size_t flops_requested = (double)LENGTH * (double)LENGTH* (double)LENGTH * 2;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init<base_type, array_type>(a, b, c, LENGTH);
#else
    int iterations = LOC_REPEAT;
#endif

	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type>(a, b, c, LENGTH);
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
#endif

		CallKernel<base_type, array_type> (core_type, a, b, c, LENGTH);

#ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
#endif
	}

#ifndef METRIC_RUN
    counter.print_average_counters(true);
#endif
}

extern "C" int main()
{
    CallKernel((int)MODE);
    return 0;
}

