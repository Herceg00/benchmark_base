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
	auto counter = PerformanceCounter();

    size_t bytes_requested = (long int) LENGTH * ( sizeof(double));
    size_t flops_requested = (long int) LENGTH;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>();

		locality::utils::CacheAnnil(3);

        counter.start_timing();

		Kernel<base_type, array_type, helper_type> (a, LENGTH);

        counter.end_timing();

		counter.update_counters(bytes_requested, flops_requested);

		counter.print_local_counters();

		std::swap(a,b);
	}
	counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) flops_requested / (double) bytes_requested<< " flops/byte";
	return time;
}

int main()
{
    std::cout << "Array size: " << (long int) LENGTH << std::endl;
	LOC_PAPI_INIT

    double time = CallKernel();

    locality::plain::Print(LENGTH, time);
}
