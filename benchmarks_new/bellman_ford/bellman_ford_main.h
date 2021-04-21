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


double CallKernel(int core_type)
{
    // Declare graph in optimized Vect CSR representation
    VectCSRGraph graph;

    double time = -1;

    #ifdef METRIC_RUN
    int iterations = LOC_REPEAT * GRAPH_METRICS_REPEAT;
    #else
    int iterations = LOC_REPEAT;
    #endif
    Init(graph, LENGTH);
    VerticesArray<float> distances(graph);
    EdgesArray_Vect<float> weights(graph);
    weights.set_all_random(1.0);

    #ifndef METRIC_RUN
    auto counter = PerformanceCounter(INT_ELEMENTS_PER_EDGE, 1);
    #endif

    for(int i = 0; i < iterations; i++)
    {
        #ifndef METRIC_RUN
        locality::utils::CacheAnnil();
        #endif

        AlgorithmStats stats = Kernel(core_type, graph, distances, weights);

        #ifndef METRIC_RUN
        counter.force_update_counters(stats.wall_time, stats.band_per_iteration, stats.wall_perf);
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
    CallKernel((int)MODE);
}
