#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;

#include "stencil_2D.h"
#include "../../locutils_new/timers.h"

double CallKernel(int mode)
{
    base_type *a = new base_type[(size_t)LENGTH * (size_t)LENGTH];
    base_type *b = new base_type[(size_t)LENGTH * (size_t)LENGTH];
    std::cout << "array sizes: " << ((size_t)LENGTH * (size_t)LENGTH / 1e9) * sizeof(base_type)* 2 << std::endl;

	double time = -1;

    #ifndef METRIC_RUN
    size_t bytes_requested = 0;
    size_t flops_requested = 0;
    if(mode == 0) // rectangle
    {
        size_t bytes_per_element = (2*RADIUS + 1)*(2*RADIUS + 1)*sizeof(base_type); // no *2 since only 1 array in inner loop
        size_t flops_per_element = (2*RADIUS + 1)*(2*RADIUS + 1);
        bytes_requested = bytes_per_element * LENGTH* LENGTH;
        flops_requested = flops_per_element * LENGTH* LENGTH;
    }
    else if(mode == 1) // cross
    {
        size_t bytes_per_element = (2*RADIUS + 2*RADIUS + 1)*sizeof(base_type); // no *2 since only 1 array in inner loop
        size_t flops_per_element = (2*RADIUS + 2*RADIUS);
        bytes_requested = bytes_per_element * LENGTH* LENGTH;
        flops_requested = flops_per_element * LENGTH* LENGTH;
    }

    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
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

		Kernel(mode, a, b, LENGTH);

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

	delete []a;
	delete []b;

    return time;
}

extern "C" int main()
{
    CallKernel((int)MODE);
}
