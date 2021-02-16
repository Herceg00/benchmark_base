#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "../../locutils_new/perf_wrapper.h"


typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "stencil_1D.h"
#include "../../locutils_new/timers.h"



double CallKernel(void )
{
	static array_type a;
	static array_type b;


	double time = -1;
	auto counter = PerformanceCounter();

    size_t bytes_requested = (long int) LENGTH * (RADIUS + 1) * ( sizeof(double));
    size_t flops_requested = 2  * (RADIUS + 1) * (long int)LENGTH;
    int iterations;
#ifndef METRIC_RUN
    iterations = LOC_REPEAT;
#endif
#ifdef METRIC_RUN
        std::cout << "KEKEKEKE"<< std::endl;
        iterations = 1000;
        Init<base_type, array_type, helper_type>(a, b, LENGTH);
#endif


	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<base_type, array_type, helper_type>(a, b, LENGTH);

		locality::utils::CacheAnnil(3);
#endif

		counter.start_timing();

		Kernel<base_type, array_type, helper_type> (a, b, LENGTH);

		counter.end_timing();

		counter.update_counters(bytes_requested, flops_requested);

#ifndef METRIC_RUN
		counter.print_local_counters();
#endif

		std::swap(a,b);
	}
	counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) flops_requested / (double) bytes_requested<< " flops/byte";
	return time;
}

int main()
{
    std::cout << "Array size: " << (long int) LENGTH << std::endl;
    std::cout << "Radius: " << (long int) RADIUS << std::endl;
	LOC_PAPI_INIT

    double time = CallKernel();

//	System sys;
//	sys.profile(CallKernel);
//
    locality::plain::Print(LENGTH, time);

}
