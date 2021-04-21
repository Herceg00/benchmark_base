#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <typeinfo>
#include <math.h>
#include <sys/time.h>
#include <stdlib.h>

#define UNSAFE_MODE

#include "locality.h"
#include "size.h"

#define N SIZE_PREDEF
static const int SPARSITY = 10;

typedef double base_type;
typedef locality::PtrWrapper<base_type, 1> data_arr;

typedef size_t index_type;
typedef locality::PtrWrapper<index_type, 1> index_arr;

#include "s_matrix_vect_mult_diag.h"

double CallKernel()
{
	SparseDiagMatrixSpec spec = GenerateSpec(N, SPARSITY);

	SparseDiagMatrix<data_arr, index_arr> matrix;

	matrix.a = locality::PtrWrapper<base_type, 1>("a", DIM1(spec.a));
	matrix.b = locality::PtrWrapper<base_type, 1>("b", DIM1(spec.b));
	matrix.x = locality::PtrWrapper<base_type, 1>("x", DIM1(spec.x));

	matrix.ip = locality::PtrWrapper<index_type, 1>("ip", DIM1(spec.ip));
	matrix.ia = locality::PtrWrapper<index_type, 1>("ia", DIM1(spec.ia));
	matrix.ic = locality::PtrWrapper<index_type, 1>("ic", DIM1(spec.ic));

	timeval start, end;

	GenerateMatrix(matrix, spec);

	locality::utils::CacheAnnil();

locality::profiler::Logging(true);
gettimeofday(&start, NULL);

	Kernel<base_type, index_type, data_arr, index_arr>(matrix, spec);

gettimeofday(&end, NULL);
locality::profiler::Logging(false);

	printf("                  check_sum: %lg\n", Check<base_type, index_type, data_arr, index_arr>(matrix, spec));

	return locality::utils::TimeDif(start, end);
}

extern "C" int main()
{
#ifdef LOC_LOG_PATH
	locality::memory_trace::Init(LOC_LOG_PATH);
#endif

#ifdef LOC_CVG_PARAMS
	locality::cvg::Init(LOC_CVG_PARAMS);
#endif
	double time = CallKernel();

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(N, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(N, time);
#endif

	return 0;
}
