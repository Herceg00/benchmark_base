#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"

static const int EDGES_PER_VERTEX = 16;

#include "bellman_ford.h"
#include "../../locutils_new/timers.h"

void CallKernel(int mode)
{
    // Declare graph in optimized Vect CSR representation
    VectCSRGraph graph;

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * GRAPH_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif
    Init(mode, graph, LENGTH);
    VerticesArray<double> distances(graph);
    EdgesArray_Vect<double> weights(graph);
    weights.set_all_random(1.0);

    #ifndef METRIC_RUN
    auto counter = PerformanceCounter(INT_ELEMENTS_PER_EDGE, 1);
    #endif

    for(int i = 0; i < iterations; i++)
    {
        #ifndef METRIC_RUN
        locality::utils::CacheAnnil();
        #endif

        performance_stats.reset_timers();
        Kernel(graph, distances, weights);
        performance_stats.update_timer_stats();

        double perf = performance_stats.get_avg_perf(graph.get_edges_count());
        double bw = performance_stats.get_sustained_bandwidth();
        double time = performance_stats.get_inner_time();

        #ifndef METRIC_RUN
        counter.force_update_counters(time, bw, perf);
        counter.print_local_counters();
        #endif
    }

    #ifndef METRIC_RUN
    counter.print_average_counters(true);
    #endif
}

extern "C" int main()
{
    CallKernel((int)MODE);
    return 0;
}
