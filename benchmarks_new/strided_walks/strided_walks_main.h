#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define STRIDE RADIUS

typedef double base_type;

#include "strided_walks.h"
#include "../../locutils_new/timers.h"


double CallKernel(int core_type)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];

	double time = -1;
    #ifndef METRIC_RUN
    size_t flops_requested = LENGTH * 2;
    size_t bytes_requested = LENGTH * (2 * sizeof(base_type));
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    Init(a, b, LENGTH);

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

		Kernel(core_type, a, b, LENGTH);

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
	delete []b;

    return time;
}

extern "C" int main(int argc, char *argv[])
{
    CallKernel((int)MODE);
    return 0;
}
