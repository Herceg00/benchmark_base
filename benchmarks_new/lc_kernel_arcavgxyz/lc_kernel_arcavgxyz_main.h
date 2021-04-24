#include <stdio.h>
#include <sys/time.h>
#include <cmath>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef float base_type;

#include "lc_kernel_arcavgxyz.h"
#include "../../locutils_new/timers.h"
void CallKernel(int mode)
{
    cusizevector plsize, tick, half_plsize;
    plsize.x = (int)LENGTH;
    plsize.y = (int)LENGTH;
    plsize.z = (int)LENGTH;
    tick.x = rand()%2;
    tick.y = rand()%2;
    tick.z = rand()%2;
    half_plsize.x = plsize.x/2;
    half_plsize.y = plsize.y/2;
    half_plsize.z = plsize.z/2;

    #ifndef METRIC_RUN
    size_t problem_size = (size_t)LENGTH * (size_t)LENGTH * (size_t)LENGTH;
    size_t bytes_requested = 16 * sizeof(base_type) * problem_size / 8;
    size_t flops_requested = 182 * problem_size / 8;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #endif

    #ifndef METRIC_RUN
    int iterations = LOC_REPEAT;
    #endif
    slpointers *sldata_array = new slpointers [1];
    tlpointers *tldata_array = new tlpointers [1];
    Init(tldata_array[0], sldata_array[0], plsize, half_plsize);

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        Init(tldata_array[0], sldata_array[0], plsize, half_plsize);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		Kernel(mode, plsize, half_plsize,tick, &sldata_array[0], &tldata_array[0]);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    #endif
}

extern "C" int main()
{
    CallKernel((int)MODE);
    return 0;
}
