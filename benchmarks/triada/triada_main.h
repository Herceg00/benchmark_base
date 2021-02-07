#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define TYPE TYPE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "triada.h"

class PerformanceCounter {
    double total_time = 0;
    double total_bw = 0;
    double total_flops = 0;
//    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
//    std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();
    double start_time = omp_get_wtime();
    double end_time = omp_get_wtime();
    double local_bw = 0;
    double local_time = 0;
    double local_flops = 0;

public:


    void start_timing(void) {
//        start_time = std::chrono::steady_clock::now();
        start_time = omp_get_wtime();
    }

    void end_timing(void) {
//        end_time = std::chrono::steady_clock::now();
        end_time = omp_get_wtime();
    }

    void update_counters(size_t bytes_requested, size_t flops_executed) {
//        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        double elapsed_seconds = end_time - start_time;
//        local_time = elapsed_seconds.count();
        local_time = elapsed_seconds;
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
    int triad_step_size[16] = {2,3,3,4,3,4,4,5,3,4,4,5,2,2,3,4};
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;


	double time = -1;
    size_t bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(size_t));
    auto counter = PerformanceCounter();

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);

		locality::utils::CacheAnnil(core_type);

        counter.start_timing();

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, LENGTH);

#pragma omp barrier

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

	for(int core_type = (int)MODE; core_type < (int)MODE + 1 ; core_type++)
	{
		//locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

		double time = CallKernel(core_type);

		locality::plain::Print(LENGTH, time);
	}
}
