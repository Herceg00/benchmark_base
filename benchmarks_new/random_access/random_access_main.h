#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;
typedef base_type array_type[(int)LENGTH];
typedef base_type indirect_type[(int)RADIUS];
typedef size_t helper_type[(int)LENGTH];

#include "random_access.h"
#include "../../locutils_new/timers.h"



double CallKernel(void )
{
	static array_type a;
	static array_type index;
    static indirect_type data;

	double time = -1;

#ifndef METRIC_RUN
    size_t bytes_requested = (long int) LENGTH * (RADIUS + 1) * ( sizeof(double));
    size_t flops_requested = 2  * (RADIUS + 1) * (long int)LENGTH;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init<base_type, array_type,indirect_type,  helper_type>(a, index, data , (int)LENGTH);
#else
    int iterations = LOC_REPEAT;
#endif

    for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
        Init<base_type, array_type,indirect_type,  helper_type>(a, index, data , (int)LENGTH);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
#endif

		Kernel<base_type, array_type, indirect_type, helper_type> (a, index, data, (int)LENGTH);

#ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
#endif

	}

#ifndef METRIC_RUN
	counter.print_average_counters(true);
#endif
	return time;
}

int main()
{
    CallKernel();
}
