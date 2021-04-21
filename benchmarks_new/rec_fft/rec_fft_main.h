#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>
#include "omp.h"
#include <chrono>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[N][2];

#include "rec_fft.h"
#include "../../locutils_new/timers.h"

double CallKernel()
{
	static array_type x;
	static array_type X;
	static array_type XX;

#ifndef METRIC_RUN
    size_t bytes_requested = 8 * sizeof(base_type) * ((size_t)LENGTH / 2  + (log2((size_t)LENGTH) - 1) * (size_t)LENGTH / 2);
    size_t flops_requested = 5 * (size_t)LENGTH / 2 + 18 * (log2((int)LENGTH) - 1) * (size_t)LENGTH / 2;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

    int iterations;

#ifdef METRIC_RUN
    iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    Init<base_type, array_type>(x, X, XX, LENGTH);
#else
    iterations = LOC_REPEAT;
#endif

	double time = -1;

	for(int i = 0; i < iterations; i++)
	{

#ifndef METRIC_RUN
		Init<base_type, array_type>(x, X, XX, LENGTH);
		locality::utils::CacheAnnil();
#endif

		Kernel<base_type, array_type>(0, 1, x, X, XX, LENGTH);

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

extern "C" int main()
{
	CallKernel();
	
	return 0;
}
