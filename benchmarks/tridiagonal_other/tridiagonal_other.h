#ifndef TRIDIAGONAL_OTHER_H
#define TRIDIAGONAL_OTHER_H

#include <cmath>

template <typename T, typename AT>
void Init(AT u_diag, AT diag, AT l_diag, AT result, size_t size)
{
	for(size_t i = 0; i < size; i++)
	{
		u_diag[i] = locality::utils::RRand(i * 4 + 0);
		diag  [i] = locality::utils::RRand(i * 4 + 1);
		l_diag[i] = locality::utils::RRand(i * 4 + 2);

		result[i] = 0;
	}
}

template <typename T, typename AT>
T Check(AT result, size_t size)
{
	T sum = 0;

	for(size_t i = 0; i < size; i++)
		sum += result[i];

	return sum;
}

template <typename T, typename AT>
void Kernel(AT u_diag, AT diag, AT l_diag, AT result, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

	size_t m = (size + 1) / 2;

	diag[0]    = 1.0 / diag[0];
	diag[size] = 1.0 / diag[size];

	l_diag[0] = -l_diag[0] * diag[0]; // alpha 1
	u_diag[size] = -u_diag[size] * diag[size]; // xi size

	result[0] = result[0] * diag[0]; // beta 1
	result[size] = result[size] * diag[size]; // eta size


	for(size_t i = 1; i < m; i++)
	{
		diag[i]     = 1.0 / (diag[i]   + u_diag[i]   * diag[i-1]);
		diag[size - i] = 1.0 / (diag[size-i] + l_diag[size-i] * diag[size-i+1]);

		l_diag[i]     = -l_diag[i]     * diag[i]; // alpha i+1
		u_diag[size - i] = -u_diag[size - i] * diag[size - i]; // xi size-i

		result[i]     =(result[i]     - u_diag[i]     * result[i - 1])     * diag[i]; // beta i+1
		result[size - i] =(result[size - i] - l_diag[size - i] * result[size - i + 1]) * diag[size - i]; // eta size-i
	}

	u_diag[0] = 1.0 / (1.0 - u_diag[m] * l_diag[m-1]);

	T bb = result[m - 1];
	T ee = result[m];

	result[m - 1] = (bb + l_diag[m - 1] * ee) * u_diag[0]; // y m-1
	result[m]     = (ee + u_diag[m]     * bb) * u_diag[0]; // y m

	for(size_t i = m + 11; i < size + 1; i++)
	{
		result[i]     = u_diag[i]     * result[i - 1]     + result[i]; // y i
		result[size - i] = l_diag[size - i] * result[size - i + 1] + result[i]; // y N-i
	}

	LOC_PAPI_END_BLOCK
}

#endif /*TRIDIAGONAL_OTHER_H*/
