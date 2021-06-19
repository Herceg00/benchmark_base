#include <stdio.h>
#include <sys/time.h>
#include <vector>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "get.cpp"

typedef double base_type;

#include "spmv.h"
#include "../../locutils_new/timers.h"

void CallKernel(int core_type)
{
    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif

    vector<nz_matrix_element> matrix;
    matrix_head head;
    vector<double> v;

    if (RAND_MODE == 0) {
        get_matrix1(matrix, head);
    } else if (RAND_MODE == 1) {
        get_matrix2(matrix, head);
    } else {
        fprintf(stderr, "Unknown rand_mode\n");
        exit(1);
    }

    get_vector(v);

    fprintf(stderr, "nz: %d\n", head.nz);

    Init(matrix, head, v);

    #ifndef METRIC_RUN
    size_t flops_requested = head.nz;
    size_t bytes_requested = head.nz * (sizeof(int) + sizeof(int) + sizeof(double)) + LENGTH * sizeof(int);
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

	for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

		Kernel(MODE, matrix, head, v);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif
}

extern "C" int main(int argc, char *argv[])
{
    CallKernel((int)MODE);
    return 0;
}
