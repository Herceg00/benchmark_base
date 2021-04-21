#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>


#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type vector_type[N];

#include "cyclic_reduction.h"

double CallKernel()
{
	static vector_type u_diag, diag, l_diag, right, result;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, vector_type>(u_diag, diag, l_diag, right, result, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, vector_type>(u_diag, diag, l_diag, right, result, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, vector_type>(result, N));

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
