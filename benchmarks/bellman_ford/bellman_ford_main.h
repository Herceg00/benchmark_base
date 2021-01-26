#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "omp.h"


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

static const int EDGES_PER_VERTEX = 16;

typedef size_t** edge_type;
typedef size_t* index_type;
typedef int* weight_type;

#include "bellman_ford.h"

double CallKernel()
{
	size_t edge_count = std::pow(2, N) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, N);

	edge_type edges = new size_t*[edge_count];

	for(size_t i = 0; i < edge_count; i++)
		edges[i] = new size_t[2];

	index_type index = new size_t[vertex_count];

	weight_type weights = new int[edge_count];
	weight_type d = new int[vertex_count];

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

        double time_start = omp_get_wtime();

		Kernel<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, d);

        double time_end = omp_get_wtime();

        printf("Time spent %lf", time_end - time_start);

        size_t bytes_requested = edge_count * vertex_count * (4 * sizeof(int) + 2 * sizeof(size_t) + sizeof(int)) + vertex_count;

        printf("Memory bandwidth %lf", bytes_requested * 1e-9 / (time_end - time_start));


gettimeofday(&end, NULL);

		printf("                  check_sum: %ld\n", Check<weight_type>(d, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
