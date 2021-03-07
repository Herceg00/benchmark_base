#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;

#include "stencil_1D.h"
#include "../../locutils_new/timers.h"

double CallKernel(void)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];

	double time = -1;

    #ifndef METRIC_RUN
    size_t bytes_requested = (size_t) LENGTH * (2*RADIUS + 1) * sizeof(double);
    size_t flops_requested = (2*RADIUS + 1) * (size_t)LENGTH;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init(a, b, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		Init(a, b, LENGTH);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		Kernel(a, b, LENGTH);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif

		std::swap(a,b);
	}

    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    #endif
	return time;

	delete []a;
	delete []b;
}

int main()
{
    CallKernel();
}
