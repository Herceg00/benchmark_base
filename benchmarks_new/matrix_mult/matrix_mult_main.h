#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>


typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_mult.h"
#include "../../locutils_new/timers.h"

using namespace matrix_mult;


double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
    double bw_for_stat[6] = {(double)LENGTH * (double)LENGTH* (double)LENGTH * 2  + 2 * (double)LENGTH * (double)LENGTH,
                              (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                              (double)LENGTH * (double)LENGTH* (double)LENGTH * 2  + 2 * (double)LENGTH * (double)LENGTH,
                              (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                              (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH,
                              (double)LENGTH * (double)LENGTH* (double)LENGTH * 3  + 1 * (double)LENGTH * (double)LENGTH};


	double time = -1;
#ifndef METRIC_RUN
    double bytes_requested = sizeof(double) * bw_for_stat[core_type];
    double flops_requested = (double)LENGTH * (double)LENGTH* (double)LENGTH * 2;
    auto counter = PerformanceCounter();
#endif
    int iterations;
#ifdef METRIC_RUN
    iterations = LOC_REPEAT * 20;
    Init<base_type, array_type>(a, b, c, LENGTH);
#endif
#ifndef METRIC_RUN
    iterations = LOC_REPEAT;
#endif

	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type>(a, b, c, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();
#endif
		CallKernel<base_type, array_type> (core_type, a, b, c, LENGTH);
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

	for(int core_type = (int)MODE; core_type < (int)MODE + 1; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}

