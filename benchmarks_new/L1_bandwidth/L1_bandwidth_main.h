#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <omp.h>
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"

typedef float base_type;

#include "L1_bandwidth.h"
#include "../../locutils_new/timers.h"

#ifdef __USE_INTEL__
#define SIMD_SIZE 512
#endif

#ifdef __USE_KUNPENG__
#define SIMD_SIZE 128
#endif

#define INNER_LOADS 16

void CallKernel(int mode)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];
    float **chunk_read = (float **)malloc(sizeof(float *) * omp_get_max_threads());
    float **chunk_write = (float **)malloc(sizeof(float *) * omp_get_max_threads());

    std::cout << 4 * sizeof(float) << std::endl;
    #ifndef METRIC_RUN
    size_t bytes_requested = (size_t) RADIUS * (SIMD_SIZE/sizeof(float)) * (size_t)INNER_LOADS * omp_get_max_threads();
    if (mode == 1) {
        bytes_requested *= 2;
    }
    size_t flops_requested = (size_t) RADIUS * (size_t)INNER_LOADS * omp_get_max_threads();
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    Init(a, b, chunk_read, chunk_write, LENGTH);

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        std::cout << "cache anych" << std::endl;
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		float val = Kernel(mode, chunk_read, chunk_write, LENGTH);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif

		std::cout << "val: " << val << std::endl;
	}
    
    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    counter.print_bw();
    counter.print_flops();
    #endif

	delete []a;
	delete []b;
	free(chunk_read);
	free(chunk_write);
}

extern "C" int main()
{
    omp_set_num_threads(THREADS);
    CallKernel((int)MODE);
    return 0;
}
