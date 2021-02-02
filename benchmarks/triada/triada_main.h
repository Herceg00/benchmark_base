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

double CallKernel(int core_type)
{
    int triad_step_size[16] = {2,3,3,4,3,4,4,5,3,4,4,5,2,2,3,4};
	static array_type a;
	static array_type b;
	static array_type c;
	static array_type x;
	static helper_type ind;
    double total_time = 0;
    double total_bw = 0;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type, helper_type>(core_type, a, b, c, x, ind, LENGTH);

		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		Kernel<base_type, array_type, helper_type> (core_type, a, b, c, x, ind, LENGTH);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;

        std::cout << "local_time: " << elapsed_seconds.count() << " s\n";

        total_time += elapsed_seconds.count();

        size_t bytes_requested = LENGTH * (triad_step_size[(int)MODE] * sizeof(size_t));

        double local_bw = bytes_requested * 1e-9 / elapsed_seconds.count();
        total_bw += local_bw;
        printf("local_bw: %lf GB/s\n", local_bw);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(a, LENGTH));

		double next_time = locality::utils::TimeDif(start, end);

//		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}
    std::cout << "avg_time: " << total_time/LOC_REPEAT << " s\n";
    std::cout << "avg_bw: " << total_bw / LOC_REPEAT << " Gb/s\n";
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
