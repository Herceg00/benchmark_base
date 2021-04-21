#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type vec_type[N];
typedef base_type matrix_type[N][N];

#include "d_matrix_vect_mult.h"

double CallKernel()
{
	static matrix_type matrix;
	static vec_type vec_in;
	static vec_type vec_out;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, vec_type, matrix_type>(vec_in, vec_out, matrix, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, vec_type, matrix_type>(vec_in, vec_out, matrix, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, vec_type>(vec_out, N));

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