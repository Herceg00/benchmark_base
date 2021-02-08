#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>


typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_mult.h"

using namespace matrix_mult;

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

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
    int bw_for_stat[6] = {(int)LENGTH * (int)LENGTH* (int)LENGTH * 2  + 2 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 2  + 2 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH,
                              (int)LENGTH * (int)LENGTH* (int)LENGTH * 3  + 1 * (int)LENGTH * (int)LENGTH};


	double time = -1;
    size_t bytes_requested = sizeof(double) * bw_for_stat[core_type];
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, c, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		CallKernel<base_type, array_type> (core_type, a, b, c, LENGTH);

        counter.end_timing();

        counter.update_counters(bytes_requested, 0);

        counter.print_local_counters();

	}

    counter.print_average_counters(false);
    return time;
}

int main()
{
	LOC_PAPI_INIT

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}

