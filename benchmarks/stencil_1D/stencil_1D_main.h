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

        std::cout << "\n\nelapsed time: " << elapsed_seconds.count() << "s\n";

        size_t bytes_requested = (long int) LENGTH * ( sizeof(double));

        printf("Minimal memory bandwidth %lf GB/s\n", bytes_requested * 1e-9 / elapsed_seconds.count());
        printf("Maximal memory bandwidth %lf GB/s\n", 2 * (RADIUS + 1) * bytes_requested * 1e-9 / elapsed_seconds.count());
        printf("Performance  %lf GFlops ", 2  * (RADIUS + 1) * LENGTH * 1e-9 / elapsed_seconds.count());

gettimeofday(&end, NULL);

//		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
		std::swap(a,b);
	}

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
