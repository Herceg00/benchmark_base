#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

typedef double base_type;

#include "cache_bandwidths.h"
#include "../../locutils_new/timers.h"

double CallKernel(int core_type)
{
    const int threads = omp_get_max_threads();
    base_type **private_caches_data = new base_type*[threads];
    for(int tid = 0; tid < threads; tid++)
        private_caches_data[tid] = new base_type[LENGTH];

	double time = -1;
    #ifndef METRIC_RUN
    size_t flops_requested = (size_t)threads * LENGTH * STEPS_COUNT;
    size_t bytes_requested = (size_t)threads * LENGTH * STEPS_COUNT * sizeof(base_type);
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init(private_cache, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        Init(private_caches_data, LENGTH);
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

        Kernel(0, private_caches_data, LENGTH);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif

    for(int tid = 0; tid < threads; tid++)
        delete []private_caches_data[tid];
    delete []private_caches_data;

    std::cout << glob_accum << std::endl;

    return time;
}

int main()
{
    CallKernel((int)MODE);
}
