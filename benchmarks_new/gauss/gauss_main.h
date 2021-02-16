#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"


typedef double base_type;
typedef int indx_type[LENGTH];
typedef base_type vec_type[LENGTH];
typedef base_type matrix_type[LENGTH][LENGTH];

#include "gauss.h"
#include "../../locutils_new/timers.h"


double CallKernel()
{
	static matrix_type matrix;
	static vec_type b;
	static vec_type x;
	static indx_type indx;

	double time = -1;
#ifndef METRIC_RUN
    double bytes_requested = 3 * (double)LENGTH * (double)LENGTH * (double)LENGTH * sizeof(double)/4  + 8 * sizeof(double)*(double)LENGTH*(double)LENGTH + 17 * sizeof(double)*(double)LENGTH ;
    double local_performance = (double)LENGTH*(double)LENGTH*(double)LENGTH/2 + (double)LENGTH*(double)LENGTH * 3.5 + 2 *(double)LENGTH;
    auto counter = PerformanceCounter();
#endif
    int iterations;
#ifdef METRIC_RUN
    Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);
    iterations = 20 * LOC_REPEAT;
#endif
#ifndef METRIC_RUN
    iterations = LOC_REPEAT;
#endif


	for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
        Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

		locality::utils::CacheAnnil();

        counter.start_timing();
#endif

		Kernel<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

#ifndef METRIC_RUN
        counter.end_timing();

        counter.update_counters(bytes_requested, local_performance);

        counter.print_local_counters();
#endif
	}
#ifndef METRIC_RUN
    counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) local_performance / (double) bytes_requested<< " flops/byte";
#endif
    return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(LENGTH, time);

	return 0;
}
