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

class PerformanceCounter {
    double total_time = 0;
    double total_bw = 0;
    double total_flops = 0;
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();
    double local_bw = 0;
    double local_time = 0;
    double local_flops = 0;

public:


    void start_timing(void) {
        start_time = std::chrono::steady_clock::now();
    }

    void end_timing(void) {
        end_time = std::chrono::steady_clock::now();
    }

    void update_counters(size_t bytes_requested, size_t flops_executed) {
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        local_time = elapsed_seconds.count();
        local_bw = bytes_requested * 1e-9 / local_time;
        local_flops = flops_executed * 1e-9 / local_time;
        total_bw += local_bw;
        total_time += local_time;
        total_flops += local_flops;
    }

    void print_local_counters(void) {
        std::cout << "local_time: " << local_time << " s\n";
        std::cout << "local_bw: " << local_bw << " Gb/s\n";
        std::cout << "local_flops: " << local_flops << " GFlops\n";
    }

    void print_average_counters(bool flops_required) {
        std::cout << "avg_time: " << total_time/LOC_REPEAT << " s\n";
        std::cout << "avg_bw: " << total_bw/LOC_REPEAT << " Gb/s\n";
        if (flops_required) {
            std::cout << "avg_flops: " << total_flops/LOC_REPEAT << " GFlops\n";
        }
    }
};

double CallKernel()
{
	static matrix_type matrix;
	static vec_type b;
	static vec_type x;
	static indx_type indx;

	double time = -1;
    double bytes_requested = 3 * (double)LENGTH * (double)LENGTH * (double)LENGTH * sizeof(double)/4  + 8 * sizeof(double)*(double)LENGTH*(double)LENGTH + 17 * sizeof(double)*(double)LENGTH ;
    double local_performance = (double)LENGTH*(double)LENGTH*(double)LENGTH/2 + (double)LENGTH*(double)LENGTH * 3.5 + 2 *(double)LENGTH;
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

		locality::utils::CacheAnnil();

        counter.start_timing();

		Kernel<base_type, vec_type, matrix_type, indx_type>(b, x, matrix, indx, LENGTH);

        counter.end_timing();

        counter.update_counters(bytes_requested, local_performance);

        counter.print_local_counters();
	}
    counter.print_average_counters(true);
    std::cout << "Benchmark type: " << (double) local_performance / (double) bytes_requested<< " flops/byte";
	return time;
}

extern "C" int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(LENGTH, time);

	return 0;
}
