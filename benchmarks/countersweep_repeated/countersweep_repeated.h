#ifndef COUNTERSWEEP_REPEATED_H
#define COUNTERSWEEP_REPEATED_H

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
void Kernel(AT u_diag, AT diag, AT l_diag, AT right, AT result, int size) // size = 2m-1
{
	LOC_PAPI_BEGIN_BLOCK

	int m = (size + 1) / 2;

	result[1] = right[1] * diag[1];
	result[size - 1] = right[size - 1] * diag[size - 1];

	for(int i = 2; i < m; i++)
	{
		result[i] = (right[i] - u_diag[i] * result[i-1]) * diag[i];

		int r_i = size  - i;
		result[r_i] = (right[r_i] - l_diag[r_i] * result[r_i+1]) * diag[r_i];
	}

	result[m] = (right[m] - u_diag[m] * result[m-1] - l_diag[m] * result[m+1]) * diag[m];

	for(int i = m + 1; i < N; i++)
	{
        result[i] = u_diag[i] * result[i - 1] + result[i];

		int r_i = size  - i;
        result[r_i] = l_diag[r_i] * result[r_i + 1] + result[r_i];
	}

	LOC_PAPI_END_BLOCK
}

#endif /*COUNTERSWEEP_REPEATED_H*/
