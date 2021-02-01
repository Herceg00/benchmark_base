#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[N];
typedef size_t helper_type[N];

#include "stencil_1D.h"

double CallKernel(void )
{
	static array_type a;
	static array_type b;

	timeval start, end;

	double time = -1;

	std::cout << (int)LOC_REPEAT << std::endl;
	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(a, b, N);

		locality::utils::CacheAnnil(3);

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		Kernel<base_type, array_type, helper_type> (a, b, N);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;

        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

        size_t bytes_requested = N * (4 * sizeof(size_t));

        printf("Memory bandwidth %lf GB/s", bytes_requested * 1e-9 / elapsed_seconds.count());

gettimeofday(&end, NULL);

//		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

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

}
