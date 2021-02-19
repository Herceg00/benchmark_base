#include <stdio.h>
#include <sys/time.h>
#include <cmath>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "lc_kernel.h"
#include "../../locutils_new/timers.h"

double CallKernel(void )
{
	double time = -1;
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
    auto counter = PerformanceCounter();
    size_t bytes_requested = (long int) LENGTH * (RADIUS + 1) * ( sizeof(double));
    size_t flops_requested = 2  * (RADIUS + 1) * (long int)LENGTH;
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    slpointers *sldata_array = new slpointers [iterations];
    tlpointers *tldata_array = new tlpointers [iterations];
    for (int j = 0; j < iterations; j++){
        Init(tldata_array[j], sldata_array[j], plsize, half_plsize);
    }
#else
    int iterations = LOC_REPEAT;
    slpointers *sldata_array = new slpointers [iterations];
    tlpointers *tldata_array = new tlpointers [iterations];
#endif

    for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
        Init(tldata_array[i], sldata_array[i], plsize, half_plsize);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
#endif

		Kernel(plsize, half_plsize,tick, &sldata_array[i], &tldata_array[i]);

#ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters(bytes_requested, flops_requested);
		counter.print_local_counters();
#endif
	}

#ifndef METRIC_RUN
	counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) flops_requested / (double) bytes_requested<< " flops/byte";
#endif
	return time;
}

int main()
{
    CallKernel();
}
