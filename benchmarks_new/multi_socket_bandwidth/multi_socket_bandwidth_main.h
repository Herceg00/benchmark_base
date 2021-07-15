#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <omp.h>
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"

typedef float base_type;

#include "multi_socket_bandwidth.h"
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
    base_type *a1 = new base_type[LENGTH];
    base_type *b1 = new base_type[LENGTH];
    base_type *c1 = new base_type[LENGTH];

    base_type *a2 = new base_type[LENGTH];
    base_type *b2 = new base_type[LENGTH];
    base_type *c2 = new base_type[LENGTH];

    #ifndef METRIC_RUN
    size_t bytes_requested = (size_t) LENGTH * sizeof(base_type) * 3;
    size_t flops_requested = (size_t) LENGTH * 2;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    Init(a1, b1, c1, a2, b2, c2, LENGTH);
    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        std::cout << "cache anych" << std::endl;
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

        fprintf(stderr, "GOINT INTO THE KERNEL\n\n");
		Kernel(mode, a1, b1, c1, a2, b2, c2, LENGTH);
        fprintf(stderr, "!\n");
        fprintf(stderr, "\nCOMING BACK FROM KERNEL\n\n");

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

	delete []a1;
    delete []b1;
	delete []c1;
    delete []a2;
    delete []b2;
    delete []c2;
}

extern "C" int main()
{
    omp_set_num_threads(128);
    CallKernel((int)MODE);
    return 0;
}
