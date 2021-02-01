#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"
#include <chrono>


#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[LENGTH][LENGTH];

#include "matrix_transp.h"

double CallKernel(int core_type)
{
	static array_type a;
	static array_type b;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(a, b, LENGTH);
		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);
        auto time_start = std::chrono::steady_clock::now();

		CallKernel<base_type, array_type> (core_type, a, b, BLOCK_SIZE, LENGTH);

        auto time_end = std::chrono::steady_clock::now();


        std::chrono::duration<double> elapsed_seconds = time_end - time_start;

        std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";


        size_t bytes_requested = 2 * sizeof(double) * LENGTH * LENGTH; //чтение и сразу запись

        printf("Memory bandwidth %lf GB/s\n", bytes_requested * 1e-9 / elapsed_seconds.count());

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(b, LENGTH));

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

	for(int core_type = (int)MODE; core_type < (int)MODE + 1; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

locality::plain::Print(LENGTH, time);
	}
}

