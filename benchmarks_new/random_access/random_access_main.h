#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"

typedef double base_type;
typedef int index_type;

#define RADIUS_IN_ELEMENTS (size_t)((size_t)RADIUS * 1024 / sizeof(base_type))

#include "random_access.h"
#include "../../locutils_new/timers.h"

double CallKernel(int mode)
{
    base_type *a = new base_type[LENGTH];
    index_type *index = new index_type[LENGTH];
    base_type *data = new base_type[RADIUS_IN_ELEMENTS];

	double time = -1;

    #ifndef METRIC_RUN
    size_t bytes_requested = ((size_t)LENGTH) * (2 * sizeof(base_type) + sizeof(index_type));
    size_t flops_requested = (size_t)LENGTH;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init(a, index, data, (size_t)LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        Init(a, index, data, (size_t)LENGTH);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		Kernel(mode, a, index, data, (size_t)LENGTH);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}

	#ifndef METRIC_RUN
	counter.print_average_counters(true);
    #endif

    delete []a;
    delete []index;
    delete []data;

	return time;
}

int main()
{
    CallKernel((int)MODE);
}
