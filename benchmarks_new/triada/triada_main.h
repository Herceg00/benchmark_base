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
	size_t flops_required = LENGTH * triad_step_size[(int)MODE - 1];// TODO
    size_t bytes_requested = LENGTH * (triad_step_size[(int)MODE - 1] * sizeof(size_t));
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
	    double t1 = omp_get_wtime();
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);
        double t2 = omp_get_wtime();
        std::cout << "new init time: " << t2 - t1 << std::endl;

		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, LENGTH);

        counter.end_timing();

        counter.update_counters(bytes_requested, flops_required);

        counter.print_local_counters();

	}
    counter.print_average_counters(true);
	return time;
}

int main()
{
	LOC_PAPI_INIT

	for(int core_type = (int)MODE; core_type < (int)MODE + 1 ; core_type++)
	{
		//locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}
