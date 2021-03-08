#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"


static const int EDGES_PER_VERTEX = 16;

typedef size_t** edge_type;
typedef size_t* index_type;
typedef int* weight_type;

#include "bfs.h"
#include "../../locutils_new/timers.h"


double CallKernel(int core_type)
{
	size_t edge_count = std::pow(2, LENGTH) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, LENGTH);
    index_type *v_array;
    index_type *e_array;
    e_array = new int[edges_count];
    v_array = new int[vertices_count + 1];

	edge_type edges = new size_t*[edge_count];

	for(size_t i = 0; i < edge_count; i++)
		edges[i] = new size_t[2];

	index_type index = new size_t[vertex_count];

	weight_type levels = new int[vertex_count]; //for visited-unvisited

	double time = -1;

#ifndef METRIC_RUN
    double bytes_requested = edge_count * vertex_count * (4 * sizeof(int) + 2 * sizeof(size_t) + sizeof(int)) + vertex_count;
    double flops_requested = edge_count * vertex_count * 4;
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
#endif

#ifdef METRIC_RUN
    int iterations = LOC_REPEAT * 20;
    Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, LENGTH);
#else
    int iterations = LOC_REPEAT;
#endif

    for(int i = 0; i < iterations; i++)
	{
#ifndef METRIC_RUN
		Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, LENGTH, v_array, e_array);
		locality::utils::CacheAnnil();
        counter.start_timing();
#endif

        Kernel<edge_type, weight_type, index_type>(core_type, edges, edge_count, weights, vertex_count, d, v_array, e_array, levels);

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
    CallKernel((int)MODE);
}
