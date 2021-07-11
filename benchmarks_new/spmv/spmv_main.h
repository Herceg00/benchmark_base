#include <stdio.h>
#include <sys/time.h>
#include <vector>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>
#include "get.cpp"

#define base_type double

#include "spmv.h"
#include "../../locutils_new/timers.h"

#define nz (long long) (LENGTH) * (long long)(RADIUS)

void CallKernel(int core_type)
{
    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * USUAL_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif
    vector<base_type> v;
    fprintf(stderr, "\nMax threads: %u\n", omp_get_max_threads());
    omp_set_num_threads(omp_get_max_threads());

    fprintf(stderr, "\nAllocating memory for vectors...\n\n");
    csr matrix;
    fprintf(stderr, "Memory for vectors has been allocated\n\n");
    if (RAND_MODE == 0) {
        matrix = generate_csr_matrix_ud();
    } else if (RAND_MODE == 1) {
        matrix = generate_csr_matrix_nd();
    } else {
        fprintf(stderr, "Unknown rand_mode\n");
        exit(1);
    }
    get_vector(v);

    omp_set_num_threads(THREADS);

    Init(matrix);

    #ifndef METRIC_RUN
    unsigned long long flops_requested = nz;
    unsigned long long bytes_requested = nz * (sizeof(int) + sizeof(int) + sizeof(double)) + LENGTH * sizeof(int);
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

	for(int i = 0; i < iterations; i++)
	{
        fprintf(stderr, "%d\n", i);
        #ifndef METRIC_RUN
		locality::utils::CacheAnnil(core_type);
        counter.start_timing();
        #endif

		Kernel(MODE, matrix, v);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif
    counter.print_flops();
    counter.print_bw();
    counter.print_time();
}

extern "C" int main(int argc, char *argv[])
{
    fprintf(stderr, "main\n");
    CallKernel((int)MODE);
    return 0;
}
