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
typedef base_type array_type[N][2];

#include "rec_fft.h"

double CallKernel()
{
	static array_type x;
	static array_type X;
	static array_type XX;

	timeval start, end;
	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<base_type, array_type>(x, X, XX, N);

		locality::utils::CacheAnnil();

gettimeofday(&start, NULL);

		Kernel<base_type, array_type>(0, 1, x, X, XX, N);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n"
			, Check<base_type, array_type>(X, XX, N));

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
