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


void CallKernel(int core_type)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];
    base_type *c = new base_type[LENGTH];
    base_type *x = new base_type[LENGTH];
    size_t *ind = new index_type[LENGTH];

    base_type scalar = rand() % 100;

    #ifndef METRIC_RUN
    int triad_step_size[CORE_TYPES] = {/*stream*/2,2,3,3,/*old*/3,3,3,3,3,3};
    size_t flops_requested = LENGTH * 2;
    size_t bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(base_type));
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    Init(core_type, a, b, c, x, ind, LENGTH);

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

		Kernel(core_type, a, b, c, x, ind, LENGTH, scalar);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif
	delete []a;
	delete []b;
	delete []c;
	delete []x;
	delete []ind;
}

extern "C" int main(int argc, char *argv[])
{
    CallKernel((int)MODE);
    return 0;
}
