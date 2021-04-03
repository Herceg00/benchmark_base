#ifndef BFS_H
#define BFS_H

#include <stdexcept>
#include <cmath>
#include <limits>
#include <list>
#include <cstring>
#include <array>
#include <vector>


// --------------------------------------------- VGL ---------------------------------------------

#define __USE_MULTICORE__

//#define __PRINT_API_PERFORMANCE_STATS__
//#define __PRINT_SAMPLES_PERFORMANCE_STATS__

#define INT_ELEMENTS_PER_EDGE 5.0
#define VECTOR_ENGINE_THRESHOLD_VALUE 2147483646
#define VECTOR_CORE_THRESHOLD_VALUE 5*VECTOR_LENGTH

#include "graph_library.h"

// -----------------------------------------------------------------------------------------------


void Init(EdgesListGraph &el_graph, int scale)
{
    int v = pow(2.0, scale);
    int e = 32*v;
    //GraphGenerationAPI::random_uniform(el_graph, v, e, DIRECTED_GRAPH);
    GraphGenerationAPI::R_MAT(el_graph, v, e, 57, 19, 19, 5, UNDIRECTED_GRAPH);
}

void Kernel_atomic(EdgesListGraph &el_graph, int *in_stats, int *out_stats)
{
    int vertices_count = el_graph.get_vertices_count();
    size_t edges_count = el_graph.get_edges_count();

    int *src_ids = el_graph.get_src_ids();
    int *dst_ids = el_graph.get_dst_ids();

    #pragma omp parallel for
    for(int i = 0; i < vertices_count; i++)
    {
        in_stats[i] = 0;
        out_stats[i] = 0;
    }

    #pragma omp parallel for
    for(size_t i = 0; i < edges_count; i++)
    {
        int src_id = src_ids[i];
        int dst_id = dst_ids[i];

        #pragma omp atomic
        in_stats[src_id]++;

        #pragma omp atomic
        out_stats[dst_id]++;
    }
}

void Kernel_no_atomic(EdgesListGraph &el_graph, int *in_stats, int *out_stats)
{
    int vertices_count = el_graph.get_vertices_count();
    size_t edges_count = el_graph.get_edges_count();

    int *src_ids = el_graph.get_src_ids();
    int *dst_ids = el_graph.get_dst_ids();

    #pragma omp parallel for
    for(int i = 0; i < vertices_count; i++)
    {
        in_stats[i] = 0;
        out_stats[i] = 0;
    }

    #pragma omp parallel for
    for(size_t i = 0; i < edges_count; i++)
    {
        in_stats[src_ids[i]]++;
        out_stats[dst_ids[i]]++;
    }
}

void Kernel(int core_type, EdgesListGraph &el_graph, int *in_stats, int *out_stats)
{
    switch (core_type)
    {
        case  0:
            Kernel_no_atomic(el_graph, in_stats, out_stats);
            break;
        case  1:
            Kernel_no_atomic(el_graph, in_stats, out_stats);
            break;

        default: fprintf(stderr, "Wrong core type of atomic graphs!\n");
    }
}

#endif
