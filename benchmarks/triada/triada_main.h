#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include "omp.h"
#include <chrono>

#define N SIZE_PREDEF
#define TYPE TYPE_PREDEF

typedef double base_type;
typedef base_type array_type[N];
typedef size_t helper_type[N];

#include "triada.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;

	timeval start, end;

	double time = -1;

	std::cout << (int)LOC_REPEAT << std::endl;
	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, N);

		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, N);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;

        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

        size_t bytes_requested = N * (4 * sizeof(size_t));

        printf("Memory bandwidth %lf GB/s", bytes_requested * 1e-9 / elapsed_seconds.count());

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, N));

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

	std::cout << GetCoreName(3) << std::endl;

	for(int core_type = 3; core_type < 4 ; core_type++)
	{
		//locality::plain::Rotate("triada_" +locality::utils::ToString(core_type));

		double time = CallKernel(core_type);

		locality::plain::Print(N, time);
	}
}
