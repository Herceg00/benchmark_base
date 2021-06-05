#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define TYPE TYPE_PREDEF

typedef float base_type;
typedef size_t index_type;

#include "saxpy_satis.h"
#include "../../locutils_new/timers.h"

void CallKernel(int threads)
{
    base_type *a = new base_type[LENGTH];
    base_type *b = new base_type[LENGTH];
    base_type *c = new base_type[LENGTH];

    base_type scalar = rand() % 100;

    #ifndef METRIC_RUN
    size_t flops_requested = LENGTH * 2;
    size_t bytes_requested = LENGTH * (3 * sizeof(base_type));
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    Init(a, b, c, LENGTH);

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(3);
        counter.start_timing();
        #endif

		Kernel(a, b, c, LENGTH, scalar, threads);

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
}

extern "C" int main(int argc, char *argv[])
{
    CallKernel((int)RADIUS);
    return 0;
}
