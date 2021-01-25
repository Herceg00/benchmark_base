#ifndef TRIDIAGONAL_H
#define TRIDIAGONAL_H

#include <cmath>

template <typename T, typename AT>
void Init(AT u_diag, AT diag, AT l_diag, AT right, AT result, int size)
{
	for(int i = 0; i < size; i++)
	{
		u_diag[i] = locality::utils::RRand(i * 4 + 0);
		diag  [i] = locality::utils::RRand(i * 4 + 1);
		l_diag[i] = locality::utils::RRand(i * 4 + 2);
		right [i] = locality::utils::RRand(i * 4 + 3);

		result[i] = 0;
	}
}

template <typename T, typename AT>
T Check(AT result, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += result[i];

	return sum;
}


template <typename T, typename AT>
void Kernel(AT u_diag, AT diag, AT l_diag, AT right, AT result, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(int i = 1; i < size; i++)
	{
		T tmp = u_diag[i] / diag[i-1];

		diag[i] = diag[i] - tmp * l_diag[i-1];
		right[i] = right[i] - tmp * right[i-1];
	}

	result[size - 1] = right[size - 1] / diag[size - 1];

	for(int i = size - 2; i >= 0; i--)
		result[i] = (right[i] - l_diag[i] * result[i+1]) / diag[i];

	LOC_PAPI_END_BLOCK
}

#endif /*TRIDIAGONAL_H*/
