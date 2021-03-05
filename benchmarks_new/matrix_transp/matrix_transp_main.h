#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>
#include "../../locutils_new/timers.h"

#define BLOCK_SIZE 16

#include "matrix_transp.h"

typedef double base_type;

double CallKernel(int core_type)
{
    base_type* a = new base_type[LENGTH*LENGTH];
    base_type* b = new base_type[LENGTH*LENGTH];

	double time = -1;

    #ifndef METRIC_RUN
    size_t bytes_requested = 2.0 * sizeof(base_type) * (size_t)LENGTH * (size_t)LENGTH;
    size_t flops_requested = (size_t)LENGTH * (size_t)LENGTH;
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
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();
        #endif

		CallKernel(core_type, a, b, BLOCK_SIZE, LENGTH);

        #ifndef METRIC_RUN
        counter.end_timing();

        counter.update_counters();

        counter.print_local_counters();
        #endif
	}
    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif

	delete[]a;
	delete[]b;

    return time;
}

int main()
{
    CallKernel((int)MODE);
}

