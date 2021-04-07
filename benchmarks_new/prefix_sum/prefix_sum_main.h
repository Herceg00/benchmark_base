#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>


typedef double base_type;

#include "prefix_sum.h"

#include "../../locutils_new/timers.h"

double CallKernel(int mode)
{
    base_type *data = new base_type[LENGTH];

	double time = -1;
    #ifndef METRIC_RUN
    double bytes_requested = (size_t) LENGTH * sizeof(double);
    double flops_requested = (size_t) LENGTH;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init(data, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		Init(data, LENGTH);
		locality::utils::CacheAnnil(3);
        counter.start_timing();
        #endif
        Kernel(mode, data, LENGTH);

        #ifndef METRIC_RUN
        counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif

	delete []data;

    return time;
}

int main()
{
	CallKernel((int)MODE);
}
