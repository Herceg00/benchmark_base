#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <chrono>


#include "locality.h"
#include "size.h"


typedef double base_type;
typedef int indx_type[LENGTH];
typedef base_type vec_type[LENGTH];
typedef base_type matrix_type[LENGTH][LENGTH];

#include "gauss.h"

double CallKernel()
{
	static matrix_type matrix;
	static vec_type b;
	static vec_type x;
	static indx_type indx;
    double total_time = 0;
    double total_bw = 0;
    double total_flops = 0;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		Kernel<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;

        total_time += elapsed_seconds.count();
        std::cout << "local_time: " << elapsed_seconds.count() << " s\n";


        double bytes_requested = 3 * (double)LENGTH * (double)LENGTH * (double)LENGTH * sizeof(double)/4  + 8 * sizeof(double)*(double)LENGTH*(double)LENGTH + 17 * sizeof(double)*(double)LENGTH ;
        double local_bw = bytes_requested * 1e-9 / elapsed_seconds.count();
        total_bw += local_bw;
        printf("local_bw: %lf GB/s\n", local_bw);

        double local_performance = ((double)LENGTH*(double)LENGTH*(double)LENGTH/2 + (double)LENGTH*(double)LENGTH * 3.5 + 2 *(double)LENGTH) * 1e-9 / elapsed_seconds.count();

        printf("Performance  %lf GFlops ", local_performance);
        total_flops += local_performance;

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, vec_type>(x, LENGTH));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}
    std::cout << "avg_time: " << total_time/LOC_REPEAT << " s\n";
    std::cout << "avg_bw: " << total_bw / LOC_REPEAT << " Gb/s\n";
    std::cout << "avg_flops: " << total_flops / LOC_REPEAT << " GFlops\n";
	return time;
}

int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(LENGTH, time);

	return 0;
}
