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

#include "bellman_ford.h"
#include "../../locutils_new/timers.h"



double CallKernel()
{
	size_t edge_count = std::pow(2, LENGTH) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, LENGTH);

	edge_type edges = new size_t*[edge_count];

	for(size_t i = 0; i < edge_count; i++)
		edges[i] = new size_t[2];

	index_type index = new size_t[vertex_count];

	weight_type weights = new int[edge_count];
	weight_type d = new int[vertex_count];

	double time = -1;

    size_t bytes_requested = edge_count * vertex_count * (4 * sizeof(int) + 2 * sizeof(size_t) + sizeof(int)) + vertex_count;
    size_t flops_requested = edge_count * vertex_count * 4;

    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, LENGTH);

		locality::utils::CacheAnnil();

        counter.start_timing();

        Kernel<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, d);

        counter.end_timing();

        counter.update_counters(bytes_requested, flops_requested);

        counter.print_local_counters();
	}
    counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) flops_requested / (double) bytes_requested<< " flops/byte";
	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(LENGTH, time);

	return 0;
}
