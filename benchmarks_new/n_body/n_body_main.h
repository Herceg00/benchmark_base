#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>


#include "n_body.h"

#include "../../locutils_new/timers.h"

double CallKernel()
{
    const int nTimeSteps = 100;
    const double Mass = 1e12;
    const double dt = 1e-4;
    const unsigned numParticles = LENGTH;

    Problem problem(Mass, dt, numParticles);

	double time = -1;
    #ifndef METRIC_RUN
    double bytes_requested = (size_t) LENGTH * (size_t) LENGTH * 3 * sizeof(double);
    double flops_requested = (size_t) LENGTH * (size_t) LENGTH * 17;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(3);
        counter.start_timing();
        #endif
        Kernel(problem, nTimeSteps);

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
	CallKernel();
}