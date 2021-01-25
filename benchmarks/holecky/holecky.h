#ifndef HOLECKY_H
#define HOLECKY_H

const int BENCH_COUNT = 2;

const char* type_names[BENCH_COUNT] = {
	"holecky_1", // 0
	"holecky_2", // 1
};

template <typename T, typename AT2>
void Init(AT2 matrix, size_t size)
{
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
}

template <typename T, typename AT2>
T Check(AT2 matrix, size_t size)
{
	T sum = 0;

	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < size; j++)
			sum += matrix[i][j];

	return sum;
}

template <typename T, typename AT2>
void Kernel1(AT2 matrix, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t i = 0; i < size; i++)
	{
		T sum = matrix[i][i];

		if(i > 1) // WTF?
			for(size_t ip = 0; ip < i - 1; ip++)
			{
				T elem = matrix[i][ip];
				sum = sum - elem * elem;
			}

		matrix[i][i] = sqrt(sum);

		for(size_t j = i + 1; j < size; j++)
		{
			sum = matrix[j][i];

			if(i > 1) // WTF?
				for(size_t ip = 0; ip < i - 1; ip++)
				{
					sum = sum - matrix[i][ip] * matrix[j][ip];
				}

			matrix[j][i] = sum / matrix[i][i];
		}
	}

	LOC_PAPI_END_BLOCK
}

// https://www.dusers.drexel.edu/~julian/experience/classwork/multi-threaded-cpu-cholesky-decomposition/
template <typename T, typename AT2>
void Kernel2(AT2 matrix, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t k = 0; k < size; k++)
	{
		matrix[k][k] = sqrt(matrix[k][k]);

		// Division step
#pragma omp parallel for num_threads(LOC_THREADS)
		for(size_t j = (k + 1); j < size; j++)
			matrix[k][j] /= matrix[k][k]; // Division step

#pragma omp parallel for num_threads(LOC_THREADS)
		for(size_t i = (k + 1); i < size; i++)
		{
			for(size_t j = i; j < size; j++)
			{
				matrix[i][j] -= matrix[k][i] * matrix[k][j];
			}
		}
	}

	// As the final step, zero out the lower triangular portion of U
#pragma omp parallel for num_threads(LOC_THREADS)
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < i; j++)
			matrix[i][j] = 0.0;

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT2>
void CallKernel(int core_type, AT2 matrix, size_t size)
{
	switch(core_type)
	{
		case 0: Kernel1<T, AT2>(matrix, size); break;

		case 1: Kernel2<T, AT2>(matrix, size); break;

		default: fprintf(stderr, "unexpected core type");
	}
}


#endif
