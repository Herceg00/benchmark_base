#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

typedef double base_type;

#include "cache_bandwidths.h"
#include "../../locutils_new/timers.h"

void CallKernel()
{
    int threads_count = omp_get_max_threads();
    cout << threads_count << endl;
    base_type *caches_data = new base_type[LENGTH*threads_count];

    #ifndef METRIC_RUN
    size_t flops_requested = (size_t)LENGTH * STEPS_COUNT;
    size_t bytes_requested = (size_t)LENGTH * STEPS_COUNT * sizeof(base_type);
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init(caches_data, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

    iterations = 1;
	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        Init(caches_data, LENGTH);
		locality::utils::CacheAnnil();
        counter.start_timing();
        #endif

        Kernel(caches_data, LENGTH);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif

    delete []caches_data;
}

extern "C" int main()
{
    CallKernel();
    return 0;
}
