#include <stdio.h>
#include <sys/time.h>
#include <cmath>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef float base_type;
#define INNER_COMPLEXITY RADIUS

#include "lc_kernel_generic.h"
#include "../../locutils_new/timers.h"

using namespace std;

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

    base_type *out_data_linear = new base_type [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
    Coord *out_data_coord = new Coord  [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];

    base_type *in_data_linear[INNER_COMPLEXITY];
    Coord *in_data_coord[INNER_COMPLEXITY];
    for(int i = 0; i < INNER_COMPLEXITY; i++)
    {
        in_data_linear[i] = new base_type [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
        in_data_coord[i] = new Coord  [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
    }

    #ifndef METRIC_RUN
    size_t problem_size = (size_t)LENGTH * (size_t)LENGTH * (size_t)LENGTH;
    size_t inner_complexity = INNER_COMPLEXITY;
    size_t bytes_requested = inner_complexity * 8 * sizeof(base_type) * problem_size / 8;
    size_t flops_requested = inner_complexity * 8 * problem_size / 8;
    if(mode == 2 || mode == 3)
    {
        bytes_requested *= 3;
        flops_requested *= 3;
    }
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #endif

    #ifndef METRIC_RUN
    int iterations = LOC_REPEAT;
    #endif

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        if(mode == 0 || mode == 2)
            Init(in_data_linear, out_data_linear, plsize, half_plsize);
        if(mode == 1 || mode == 3)
            Init(in_data_coord, out_data_coord, plsize, half_plsize);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

        if(mode == 0 || mode == 2)
            Kernel(mode, plsize, half_plsize,tick, in_data_linear, out_data_linear);
		if(mode == 1 || mode == 3)
		    Kernel(mode, plsize, half_plsize,tick, in_data_coord, out_data_coord);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    #endif

    delete []out_data_linear;
    delete []out_data_coord;
    for(int i = 0; i < INNER_COMPLEXITY; i++)
    {
        delete[] in_data_linear[i];
        delete[] in_data_coord[i];
    }
}

extern "C" int main()
{
    CallKernel((int)MODE);
    return 0;
}
