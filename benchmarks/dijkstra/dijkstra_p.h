#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

static const int EDGES_PER_VERTEX = 16;

typedef locality::PtrWrapper<size_t, 2> edge_type;
typedef locality::PtrWrapper<size_t, 1> index_type;
typedef locality::PtrWrapper<int, 1> weight_type;

#include "dijkstra.h"

double CallKernel()
{
	size_t edge_count = std::pow(2, N) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, N);

	edge_type edges = locality::PtrWrapper<size_t, 2>("edges", DIM2(edge_count, 2));

	index_type index = locality::PtrWrapper<size_t, 1>("index", DIM1(vertex_count));

	weight_type weights = locality::PtrWrapper<int, 1>("weights", DIM1(edge_count));
	weight_type d = locality::PtrWrapper<int, 1>("d", DIM1(vertex_count));
	weight_type visited = locality::PtrWrapper<int, 1>("visited", DIM1(vertex_count));

	timeval start, end;

	Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, N);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, d, visited, 0);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("check_sum: %ld\n", Check<weight_type>(d, N));

	return locality::utils::TimeDif(start, end);
}

int main()
{
#ifdef LOC_LOG_PATH
	locality::memory_trace::Init(LOC_LOG_PATH);
#endif

#ifdef LOC_CVG_PARAMS
	locality::cvg::Init(LOC_CVG_PARAMS);
#endif

	double time = CallKernel();

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif

	return 0;
}
