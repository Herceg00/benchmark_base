#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>


typedef double base_type;
typedef base_type array_type[LENGTH];
typedef size_t helper_type[LENGTH];

#include "stencil_1D.h"

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

double CallKernel(void )
{
	static array_type a;
	static array_type b;

	timeval start, end;

	double time = -1;
	auto counter = PerformanceCounter();

    size_t bytes_requested = (long int) LENGTH * (RADIUS + 1) * ( sizeof(double));
    size_t flops_requested = 2  * (RADIUS + 1) * (long int)LENGTH;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(a, b, LENGTH);

		locality::utils::CacheAnnil(3);

		counter.start_timing();

		Kernel<base_type, array_type, helper_type> (a, b, LENGTH);

		counter.end_timing();

		counter.update_counters(bytes_requested, flops_requested);

		counter.print_local_counters();


//        printf("Minimal memory bandwidth %lf GB/s\n", bytes_requested * 1e-9 / elapsed_seconds.count());
//        printf("Maximal memory bandwidth %lf GB/s\n", 2 * (RADIUS + 1) * bytes_requested * 1e-9 / elapsed_seconds.count());



//		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

		double next_time = locality::utils::TimeDif(start, end);


		if(next_time < time || time < 0)
			time = next_time;
		std::swap(a,b);
	}

	counter.print_average_counters(true);
	return time;
}

int main()
{
    std::cout << "Array size: " << (long int) LENGTH << std::endl;
    std::cout << "Radius: " << (long int) RADIUS << std::endl;
	LOC_PAPI_INIT

    double time = CallKernel();

    locality::plain::Print(LENGTH, time);

}
