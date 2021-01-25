#include <iostream>

#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#include "size.h"

#define LOC_PAPI_MODE 1
#include "locality.h"

#define N SIZE_PREDEF

typedef double base_type;
typedef base_type vec_type[N];
typedef base_type matrix_type[N][N];

#include "jacobi.h"

void CallKernel()
{
	static matrix_type matrix;
	static vec_type f;
	static vec_type x;
	static vec_type temp_x;

	Init<base_type, vec_type, matrix_type>(matrix, f, x, temp_x, N);

	locality::utils::CacheAnnil();

locality::papi::Start();

	Kernel<base_type, vec_type, matrix_type>(matrix, f, x, temp_x, N);

locality::papi::Stop();

	printf("check_sum: %lg\n", Check<base_type, vec_type>(x, N));
}

int main()
{
locality::papi::Init(locality::papi::Set1());
	CallKernel();

	return 0;
}
