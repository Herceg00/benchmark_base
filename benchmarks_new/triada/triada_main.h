#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define TYPE TYPE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "triada.h"
#include "../../locutils_new/timers.h"



double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;

	double time = -1;
#ifndef METRIC_RUN
    int triad_step_size[16] = {2,3,3,4,3,4,4,5,3,4,4,5,2,2,3,4};
    double flops_requested = LENGTH * 2;
    double bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(double));
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);
#else
    int iterations = LOC_REPEAT;
#endif

	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
#endif

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, LENGTH);

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
}

int main()
{
    CallKernel((int)MODE);
}
