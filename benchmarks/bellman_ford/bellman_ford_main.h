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

double CallKernel()
{
	size_t edge_count = std::pow(2, LENGTH) * EDGES_PER_VERTEX;
	size_t vertex_count = std::pow(2, LENGTH);
	double total_time = 0;
    double total_bw = 0;

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
		Init<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, LENGTH);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

        auto time_start = std::chrono::steady_clock::now();

        Kernel<edge_type, index_type, weight_type>(edges, edge_count, index, weights, vertex_count, d);

        auto time_end = std::chrono::steady_clock::now();

        size_t bytes_requested = edge_count * vertex_count * (4 * sizeof(int) + 2 * sizeof(size_t) + sizeof(int)) + vertex_count;
        std::chrono::duration<double> elapsed_seconds = time_end - time_start;
        total_time += elapsed_seconds.count();
        std::cout << "local_time: " << elapsed_seconds.count() << " s\n";
        double local_bw = bytes_requested * 1e-9 / elapsed_seconds.count();
        total_bw += local_bw;


        printf("local_bw: %lf GB/s\n", local_bw);


gettimeofday(&end, NULL);

		printf("check_sum: %ld\n\n", Check<weight_type>(d, LENGTH));

		double next_time = locality::utils::TimeDif(start, end);

//		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}
    std::cout << "avg_time: " << total_time/LOC_REPEAT << " s\n";
    std::cout << "avg_bw: " << total_bw / LOC_REPEAT << " Gb/s\n";
	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(LENGTH, time);

	return 0;
}
