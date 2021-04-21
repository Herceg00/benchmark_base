#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

static const int EDGES_PER_VERTEX = 16;

#include "boruvka.h"

typedef locality::PtrWrapper<size_t, 2> edge_type;
typedef locality::PtrWrapper<SSubset, 1> subset_type;
typedef locality::PtrWrapper<int, 1> index_type;
typedef locality::PtrWrapper<int, 1> weight_type;


double CallKernel()
{
	size_t edge_count = std::pow(2, N) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, N);

	edge_type edges = locality::PtrWrapper<size_t, 2>("edges", DIM2(edge_count, 2));

	subset_type subsets = locality::PtrWrapper<SSubset, 1>("subsets", DIM1(edge_count));
	index_type cheapest = locality::PtrWrapper<int, 1>("cheapest", DIM1(edge_count));

	weight_type weights = locality::PtrWrapper<int, 1>("weights", DIM1(edge_count));

	timeval start, end;
	double time = -1;

	for(int i = 0; i < 1; i++)
	{
		Init<edge_type, weight_type>(edges, edge_count, weights, vertex_count);

		locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

		Kernel<edge_type, subset_type, index_type, weight_type>(edges, edge_count, subsets, cheapest, weights, vertex_count);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

		printf("                  check_sum: %ld\n", Check<subset_type>(subsets, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}


extern "C" int main()
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
