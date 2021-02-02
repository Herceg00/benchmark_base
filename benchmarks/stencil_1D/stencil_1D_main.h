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

double CallKernel(void )
{
	static array_type a;
	static array_type b;
    double total_time = 0;
    double total_bw = 0;
    double total_flops = 0;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(a, b, LENGTH);

		locality::utils::CacheAnnil(3);

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		Kernel<base_type, array_type, helper_type> (a, b, LENGTH);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;


        size_t bytes_requested = (long int) LENGTH * ( sizeof(double));

        total_time += elapsed_seconds.count();
        std::cout << "local_time: " << elapsed_seconds.count() << " s\n";
        double local_bw = bytes_requested * (RADIUS + 1) * 1e-9 / elapsed_seconds.count();
        total_bw += local_bw;
        printf("local_bw: %lf GB/s\n", local_bw);

//        printf("Minimal memory bandwidth %lf GB/s\n", bytes_requested * 1e-9 / elapsed_seconds.count());
//        printf("Maximal memory bandwidth %lf GB/s\n", 2 * (RADIUS + 1) * bytes_requested * 1e-9 / elapsed_seconds.count());
        printf("local_flops %lf GFlops ", 2  * (RADIUS + 1) * LENGTH * 1e-9 / elapsed_seconds.count());
        total_flops += 2  * (RADIUS + 1) * LENGTH * 1e-9 / elapsed_seconds.count();


gettimeofday(&end, NULL);

//		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
		std::swap(a,b);
	}
    std::cout << "avg_time: " << total_time/LOC_REPEAT << " s\n";
    std::cout << "avg_bw: " << total_bw / LOC_REPEAT << " Gb/s\n";
    std::cout << "avg_flops: " << total_flops / LOC_REPEAT << " GFlops\n";
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
