#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;
typedef int index_type;
typedef base_type array_type[(int)LENGTH];
typedef index_type indirect_type[(int)LENGTH];
typedef base_type data_type[(int)RADIUS];

#include "random_access.h"
#include "../../locutils_new/timers.h"



double CallKernel(void )
{
	static array_type a;
	static indirect_type index;
    static data_type data;

	double time = -1;

#ifndef METRIC_RUN
    size_t bytes_requested = (int)LENGTH * (2 * sizeof(base_type) + sizeof(index_type));
    size_t flops_requested =(int)LENGTH;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init<base_type, array_type, indirect_type, data_type>(a, index, data , (int)LENGTH);
#else
    int iterations = LOC_REPEAT;
#endif

    for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
        Init<base_type, array_type, indirect_type, data_type>(a, index, data , (int)LENGTH);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
#endif

		Kernel<base_type, array_type, indirect_type, data_type> (a, index, data, (int)LENGTH);

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
