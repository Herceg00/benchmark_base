#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <omp.h>
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"

typedef float base_type;

#include "L1_benchmark.h"
#include "../../locutils_new/timers.h"

void CallKernel(int mode)
{
    base_type *a = new base_type[LENGTH];
    float **chunk = (float **)malloc(sizeof(float *) * omp_get_num_threads());
    float32x4_t *local_sum = new float32x4_t[omp_get_num_threads()];
    #ifndef METRIC_RUN
     size_t bytes_requested = (size_t) RADIUS * 4 * sizeof(float) * 8;
    size_t flops_requested = (size_t) RADIUS * 80;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init(a, chunk, local_sum, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

    for(int i = 0; i < iterations; i++)
	{
        fprintf(stderr, "%d\n", i);
        #ifndef METRIC_RUN
        std::cout << "cache anych" << std::endl;
		Init(a, chunk, local_sum, LENGTH);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		Kernel(mode, a, chunk, local_sum, LENGTH);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}
    
    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    counter.print_bw();
    counter.print_flops();
    #endif

	delete []a;
	free(chunk);
	delete []local_sum;
}

extern "C" int main()
{
    omp_set_num_threads(THREADS);
    CallKernel((int)MODE);
    return 0;
}
