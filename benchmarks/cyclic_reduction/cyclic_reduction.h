#ifndef CYCLIC_REDUCTION_H
#define CYCLIC_REDUCTION_H

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

	int k=1;
	int k2=2;
	int kk=1; 

	int m = log(size) / log(2.0);

	diag[1] = 1./diag[1];
	diag[size-1] = 1./diag[size-1];
	l_diag[1] = l_diag[1]*diag[1];
	u_diag[size-1] = u_diag[size-1]*diag[size-1];
	right[1] = right[1]*diag[1];
	right[size-1] = right[size-1]*diag[size-1];
	
	for(int i=3; i <= N-2; i += 2)
	{
		diag[i] = 1./diag[i];
		u_diag[i] = u_diag[i]*diag[i];
		l_diag[i] = l_diag[i]*diag[i];
		right[i] = right[i]*diag[i];
	}

	for(int j = 1; j <= m-2; j++)
	{
		k=k2;

		diag[k] = diag[k] - u_diag[k]*l_diag[k-kk];
		l_diag[k] = -l_diag[k]*l_diag[k+kk];
		right[k] = right[k]- right[k-kk]*u_diag[k-kk];
		diag[size-k] = diag[size-k] - u_diag[size-k]*l_diag[size-k-kk];
		u_diag[size-k] =  - u_diag[size-k]*u_diag[size-k-kk];
		right[size-k] = right[size-k]- right[size-k-kk]*u_diag[size-k-kk];
		diag[k] = diag[k] - l_diag[k]*u_diag[k+kk];
		right[k] = right[k] - right[k+kk]*l_diag[k+kk];
		diag[size-k] = diag[size-k] - l_diag[size-k]*u_diag[size-k+kk];
		right[size-k] = right[size-k]- right[size-k+kk]*l_diag[size-k+kk];

		k2=k2*2;

		for(int i = k2; i <= N+1-k2; i += k)
		{
			diag[i] = diag[i] - u_diag[i]*l_diag[i-kk];
			right[i] = right[i]- right[i-kk]*u_diag[i-kk];
			l_diag[i] = -l_diag[i]*l_diag[i+kk];
			u_diag[i] =  - u_diag[i]*u_diag[i-kk];
			diag[i] = diag[i] - l_diag[i]*u_diag[i+kk];
			right[i] = right[i]- right[i+kk]*l_diag[i+kk];
		}

		diag[k] = 1./diag[k];
		diag[size-k] = 1./diag[size-k];
		l_diag[k] = l_diag[k]*diag[k];
		u_diag[size-k] = u_diag[size-k]*diag[size-k];
		right[k] = right[k]*diag[k];
		right[size-k] = right[size-k]*diag[size-k];

		for(int i = k2+k; i <= N+1-k2-k; i += k2)
		{
			diag[i] = 1./diag[i];
			u_diag[i] = u_diag[i]*diag[i];
			l_diag[i] = l_diag[i]*diag[i];
			right[i] = right[i]*diag[i];
		}

		kk=k;

	}
	//        m-1 shag & start of reverse

	k=k2;

	diag[k] = diag[k] - u_diag[k]*l_diag[k-kk];
	right[k] = right[k]- right[k-kk]*u_diag[k-kk];
	diag[k] = diag[k] - l_diag[k]*u_diag[k+kk];
	right[k] = right[k]- right[k+kk]*l_diag[k+kk];
	diag[k] = 1./diag[k];
	right[k] = right[k]*diag[k];

	//           start reverse       

	right[kk] = right[kk] - l_diag[kk]*right[k];

	right[k+kk] = right[k+kk] - u_diag[k+kk]*right[k];

	for(int j = m-2; j >= 1; j--)
	{
		k2 = k;
		k = kk;
		kk = kk/2;

		right[kk] = right[kk] - l_diag[kk]*right[kk+kk];
		right[size-kk] = right[size-kk] - u_diag[size-kk]*right[size-k];

		for(int i = k2+k; i <= size-k-kk; i += k2)
		{
			right[i] = right[i] - u_diag[i]*right[i-kk];
		}

		for(int i = k2+k; i < size-k-kk; i += k2)
		{
			right[i] = right[i] - l_diag[i]*right[i+kk];
		}
	}

	result[0] = right[0];

	LOC_PAPI_END_BLOCK
}

#endif /*CYCLIC_REDUCTION_H*/
