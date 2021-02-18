#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>


typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "rand_generator.h"

#include "../../locutils_new/timers.h"

double CallKernel(void )
{
	static array_type a;
	static array_type b;

	double time = -1;
#ifndef METRIC_RUN
    auto counter = PerformanceCounter();
    double bytes_requested = (long int) LENGTH * ( sizeof(double));
    double flops_requested = (long int) LENGTH;
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init<base_type, array_type, helper_type>();
#else
    int iterations = LOC_REPEAT;
#endif

	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type, helper_type>();
		locality::utils::CacheAnnil(3);
        counter.start_timing();
#endif
		Kernel<base_type, array_type, helper_type> (a, LENGTH);

#ifndef METRIC_RUN
        counter.end_timing();
		counter.update_counters(bytes_requested, flops_requested);
		counter.print_local_counters();
#endif

		std::swap(a,b);
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
