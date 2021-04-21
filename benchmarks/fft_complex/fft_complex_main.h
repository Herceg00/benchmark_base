#define _USE_MATH_DEFINES

#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#include <iostream>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
#define BLOCK_SIZE BLOCK_SIZE_PREDEF

typedef double base_type;
typedef base_type array_type[];

#include "fft_complex.h"

using namespace fft_complex;

double CallKernel()
{
	static base_type x[N * 2];
	static base_type pov[N * 2 / 2]; // GG, guess why

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(x, pov, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, array_type>(x, pov, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, array_type>(x, N));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

extern "C" int main()
{
	LOC_PAPI_INIT

	double time = CallKernel();

locality::plain::Print(N, time);

	return 0;
}
