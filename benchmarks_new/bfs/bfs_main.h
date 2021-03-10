#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"
#include "omp.h"


static const int EDGES_PER_VERTEX = 16;

typedef size_t** edge_type;
typedef size_t* index_type;
typedef size_t* array_type;
typedef int* weight_type;

#include "bfs.h"
#include "../../locutils_new/timers.h"


double CallKernel(int mode)
{
	size_t edge_count = std::pow(2, LENGTH) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, LENGTH);
    index_type v_array;
    index_type e_array;
    array_type edges_all;
    edges_all = new size_t[2*edge_count];
    e_array = new size_t[edge_count];
    v_array = new size_t[vertex_count + 1];

	edge_type edges = new size_t*[edge_count];

	for(size_t i = 0; i < edge_count; i++)
		edges[i] = &edges_all[2*i];

	index_type index = new size_t[vertex_count];

	weight_type levels = new int[vertex_count]; //for visited-unvisited


	double time = -1;

#ifndef METRIC_RUN
    double bytes_requested = edge_count * (sizeof(e_array) + sizeof(levels));
    double flops_requested = edge_count;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT;
#else
    int iterations = LOC_REPEAT;
#endif
    Init<edge_type, index_type>(edges, edge_count, index, vertex_count, LENGTH, v_array, e_array);

    double start_time_1 = omp_get_wtime();
    for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		locality::utils::CacheAnnil();
        counter.start_timing();
#endif
        if(mode == 1) {
            Kernel_parallel<edge_type, weight_type, index_type>(vertex_count, v_array, e_array, levels);
        }
        if(mode == 0) {
            Kernel_sequential<edge_type, weight_type, index_type>(vertex_count, v_array, e_array, levels);
        }

#ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
#endif
	}
    double end_time_1 = omp_get_wtime();
    std::cout << "8xKernel time"<<  end_time_1 - start_time_1 << std::endl;

#ifndef METRIC_RUN
    counter.print_average_counters(true);
#endif

    return time;
}

int main()
{
    double start_time = omp_get_wtime();
    CallKernel(1);
    double end_time = omp_get_wtime();
    std::cout << "Main time" << end_time - start_time << std::endl;
}
