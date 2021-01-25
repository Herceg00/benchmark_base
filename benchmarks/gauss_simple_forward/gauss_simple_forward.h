#ifndef GAUSS_SIMPLE_FORWARD_H
#define GAUSS_SIMPLE_FORWARD_H

template <typename T, typename AT1, typename AT2>
void Init(AT1 b, AT1 x, AT2 matrix, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		b[i] = locality::utils::RRand(i);
		x[i] = 0;

		for(size_t j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
	}
}

template <typename T, typename AT1>
T Check(AT1 x, size_t size)
{
	T sum = 0;

	for(size_t i = 0; i < size; i++)
		sum += x[i];

	return sum;
}

template <typename T, typename AT2, typename AT1>
void Kernel1(AT2 matrix, AT1 b, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(size_t i = 0; i < size - 1; i++)
	{
#pragma omp parallel for num_threads(LOC_THREADS)
		for(size_t j = i + 1; j < size; j++)
		{
			T ratio = matrix[j][i] / matrix[i][i];

			for(size_t k = i; k < size; k++)
			{
				matrix[j][k] -= (ratio * matrix[i][k]);
				b[j] -= (ratio * b[i]);
			}
		}
	}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT1, typename AT2>
void Kernel2(AT1 b, AT1 x, AT2 matrix, size_t size)
{
	for(size_t i = size - 1; i >= 0; i--)
	{
		T sum = 0;

		for(size_t j = i + 1; j < size; j++)
			sum += matrix[i][j] * x[j];

		x[i] = (b[i] - sum) / matrix[i][i];

		if(i == 0) break;
	}
}

#endif /*GAUSS_SIMPLE_FORWARD_H*/
