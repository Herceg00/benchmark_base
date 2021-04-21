#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"
#include "omp.h"

#include "atomic_graphs.h"
#include "../../locutils_new/timers.h"


double CallKernel(int mode)
{
    // Declare graph in optimized Vect CSR representation
    EdgesListGraph el_graph;

    double time = -1;

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * GRAPH_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif
    Init(el_graph, LENGTH);

    #ifndef METRIC_RUN
    size_t bytes_requested = el_graph.get_edges_count() * 4 * sizeof(int);
    size_t flops_requested = 2*el_graph.get_edges_count();
    auto counter = PerformanceCounter(bytes_requested, flops_requested);
    #endif

    int *in_stats = new int[el_graph.get_vertices_count()];
    int *out_stats = new int[el_graph.get_vertices_count()];

    for(int i = 0; i < iterations; i++)
	{
        #ifndef METRIC_RUN
        locality::utils::CacheAnnil();
        counter.start_timing();
        #endif

        Kernel(mode, el_graph, in_stats, out_stats);

        #ifndef METRIC_RUN
        counter.end_timing();
        counter.update_counters();
        counter.print_local_counters();
        #endif
	}

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif

    delete[]in_stats;
    delete[]out_stats;

    return time;
}

extern "C" int main()
{
    CallKernel((int)MODE);
}
