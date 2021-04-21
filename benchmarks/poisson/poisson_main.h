#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type input_type[N][N][N * 2];
typedef base_type service_type[N * 2];

#include "poisson.h"

double CallKernel()
{
	static input_type input;
	static service_type tmp_x, tmp_y, tmp_z;
	static service_type pov_x, pov_y, pov_z;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, service_type, input_type>(input
			, tmp_x, tmp_y, tmp_z
			, pov_x, pov_y, pov_z
			, N, N, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, service_type, input_type>(input
			, tmp_x, tmp_y, tmp_z
			, pov_x, pov_y, pov_z
			, N, N, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n"
			, Check<base_type, input_type>(input, N, N, N));

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
