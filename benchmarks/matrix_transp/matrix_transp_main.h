#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>


#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_transp.h"
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

	double time = -1;
    size_t bytes_requested = 2 * sizeof(double) * LENGTH * LENGTH;
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, LENGTH);
		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		CallKernel<base_type, array_type> (core_type, a, b, BLOCK_SIZE, LENGTH);

        counter.end_timing();

        counter.update_counters(bytes_requested, 0);

        counter.print_local_counters();

	}

    counter.print_average_counters(false);
    return time;
}

extern "C" int main()
{
	LOC_PAPI_INIT

	for(int core_type = (int)MODE; core_type < (int)MODE + 1; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

locality::plain::Print(LENGTH, time);
	}
}

