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
    int triad_step_size[16] = {2,3,3,4,3,4,4,5,3,4,4,5,2,2,3,4};
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;


	double time = -1;
#ifndef METRIC_RUN
    auto counter = PerformanceCounter();
    double flops_requested = LENGTH * 2;
    double bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(double));
#endif
    int iterations;
#ifndef METRIC_RUN
    iterations = LOC_REPEAT;
#endif
#ifdef METRIX_RUN
    iterations = LOC_REPEAT * 20;
    Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);
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
	LOC_PAPI_INIT

	for(int core_type = (int)MODE; core_type < (int)MODE + 1 ; core_type++)
	{
		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}
