#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

static const int EDGES_PER_VERTEX = 8;

#include "boruvka.h"

typedef size_t** edge_type;
typedef SSubset* subset_type;
typedef int* index_type;
typedef int* weight_type;


double CallKernel()
{
	size_t edge_count = std::pow(2, N) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, N);

	edge_type edges = new size_t*[edge_count];

	for(size_t i = 0; i < edge_count; i++)
		edges[i] = new size_t[2];

	index_type cheapest = new int[vertex_count];

	weight_type weights = new int[edge_count];
	subset_type subsets = new SSubset[vertex_count];

	timeval start, end;
	double time = -1;

	Init<edge_type, weight_type>(edges, edge_count, weights, vertex_count);

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<edge_type, subset_type, index_type, weight_type>(edges, edge_count, subsets, cheapest, weights, vertex_count);

gettimeofday(&end, NULL);

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
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
