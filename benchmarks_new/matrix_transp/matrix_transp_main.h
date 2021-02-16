#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>
#include "../../locutils_new/timers.h"


#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_transp.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;

	double time = -1;
#ifndef METRIC_RUN
    double bytes_requested = 2 * sizeof(double) * (int)LENGTH * (int)LENGTH;
    double flops_requested = (int)LENGTH * (int)LENGTH;
    auto counter = PerformanceCounter();
#endif
    int iterations;
#ifdef METRIC_RUN
    iterations = LOC_REPEAT * 20;
    Init<base_type, array_type>(a, b, LENGTH);
#endif
#ifndef METRIC_RUN
    iterations = LOC_REPEAT;
#endif

	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type>(a, b, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();
#endif
		CallKernel<base_type, array_type> (core_type, a, b, BLOCK_SIZE, LENGTH);
#ifndef METRIC_RUN
        counter.end_timing();

        counter.update_counters(bytes_requested, flops_requested);

        counter.print_local_counters();
#endif

	}
#ifndef METRIC_RUN
    counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) flops_requested / (double) bytes_requested << " flops/byte";
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

