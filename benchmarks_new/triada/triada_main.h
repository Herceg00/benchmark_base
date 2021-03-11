#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define TYPE TYPE_PREDEF

typedef double base_type;
typedef size_t index_type;

#include "triada.h"
#include "../../locutils_new/timers.h"


double CallKernel(int core_type)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];
    base_type *c = new base_type[LENGTH];
    base_type *x = new base_type[LENGTH];
    size_t *ind = new index_type[LENGTH];

	double time = -1;
    #ifndef METRIC_RUN
    int triad_step_size[CORE_TYPES] = {2,3,3,4,3,3,3,3,3,3};
    size_t flops_requested = LENGTH * 2;
    size_t bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(double));
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init(core_type, a, b, c, x, ind, LENGTH);
    #else
    int iterations = LOC_REPEAT;
    #endif

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		Init(core_type, a, b, c, x, ind, LENGTH);
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

		Kernel(core_type, a, b, c, x, ind, LENGTH);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif
	return time;

	delete []a;
	delete []b;
	delete []c;
	delete []x;
	delete []ind;
}

int main()
{
    CallKernel((int)MODE);
}
