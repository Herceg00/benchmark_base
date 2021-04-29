#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <stdexcept>
#include <cmath>
#include <limits>


// --------------------------------------------- VGL ---------------------------------------------

#define __USE_MULTICORE__

//#define __PRINT_API_PERFORMANCE_STATS__
#define __PRINT_SAMPLES_PERFORMANCE_STATS__

#define INT_ELEMENTS_PER_EDGE 8.0
#define VECTOR_ENGINE_THRESHOLD_VALUE VECTOR_LENGTH*MAX_SX_AURORA_THREADS*128
#define VECTOR_CORE_THRESHOLD_VALUE 16*VECTOR_LENGTH

#include "graph_library.h"

// -----------------------------------------------------------------------------------------------


void Init(int core_type, VectCSRGraph &graph, int scale)
{
    EdgesListGraph el_graph;
    int vertices_count = pow(2.0, scale);
    int edges_count = 32*vertices_count;

    if(core_type == 0)
        GraphGenerationAPI::random_uniform(el_graph, vertices_count, edges_count, DIRECTED_GRAPH);
    else if( core_type == 1)
        GraphGenerationAPI::R_MAT(el_graph, vertices_count, edges_count, 57, 19, 19, 5, DIRECTED_GRAPH);


    // Warning! Graph vertices is reordered and renumbered here. You can use special VGL API functions to renumber vertices.
    graph.import(el_graph);
}

template <typename _T>
void Kernel(VectCSRGraph &graph, VerticesArray<_T> &distances, EdgesArray_Vect<_T> &weights)
{
    int source_vertex = graph.select_random_vertex(ORIGINAL);
    ShortestPaths::nec_dijkstra(graph, weights, distances, source_vertex,
                                ALL_ACTIVE, PULL_TRAVERSAL);
}

#endif
