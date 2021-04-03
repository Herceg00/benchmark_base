#include <stdio.h>
#include <sys/time.h>
#include <cmath>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef float base_type;

#include "lc_kernel_generic.h"
#include "../../locutils_new/timers.h"

double CallKernel(int mode)
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

    base_type *in_data_linear = new base_type [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
    base_type *out_data_linear = new base_type [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
    Coord *in_data_coord = new Coord [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];
    Coord *out_data_coord = new Coord  [(plsize.x+1)*(plsize.y+1)*(plsize.z+1)];

    #ifndef METRIC_RUN
    size_t problem_size = (size_t)LENGTH * (size_t)LENGTH * (size_t)LENGTH;
    size_t bytes_requested = 8 * sizeof(base_type) * problem_size / 8;
    size_t flops_requested = 8 * problem_size / 8;
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
    Init(in_data_linear, out_data_linear, plsize, half_plsize);
    Init(in_data_coord, out_data_coord, plsize, half_plsize);

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        Init(in_data_linear, out_data_linear, plsize, half_plsize);
        Init(in_data_coord, out_data_coord, plsize, half_plsize);
		locality::utils::CacheAnnil(3);
		counter.start_timing();
        #endif

		Kernel(mode, plsize, half_plsize,tick, in_data_linear, out_data_linear, in_data_coord, out_data_coord);

        #ifndef METRIC_RUN
		counter.end_timing();
		counter.update_counters();
		counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
	counter.print_average_counters(true);
    #endif

    delete []in_data_linear;
    delete []out_data_linear;
    delete []in_data_coord;
    delete []out_data_coord;

	return time;
}

int main()
{
    CallKernel((int)MODE);
}
