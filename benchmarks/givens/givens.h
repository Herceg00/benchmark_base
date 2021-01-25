#ifndef GIVENS_H
#define GIVENS_H

#include <cmath>

template <typename T, typename AT2>
void Init(AT2 matrix, int size)
{
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
}

template <typename T, typename AT2>
T Check(AT2 matrix, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			sum += matrix[i][j];

	return sum;
}

template <typename T>
void Rot2d(T& c, T& s, T& x, T& y)
{
	T tmp = c * x - s * y;
	y = s * x + c * y;
	x = tmp;
}

static const double eps = 1e-10;

template <typename T>
bool AEQ(T d1, T d2)
{
	return std::abs(d1 - d2) < eps;
}

/**
	returns value of a with sign of b
*/
template <typename T>
T SIGN(T a, T b)
{
	return b >= 0 ? std::abs(a) : -std::abs(a);
}

template <typename T>
void Params(T& x, T& y, T& c, T& s)
{
	T tmp = std::max(std::abs(x), std::abs(y));

	if(AEQ<T>(tmp, 0.0))
	{
		c = 1.0;
		s = 0.0;
	}
	else if(AEQ<T>(tmp, std::abs(x)))
	{
		T r = y / x;
		T rr = r * r;
		T rr2 = std::sqrt(1.0 + rr);

        x = x * rr2;
        y = (1.0 - rr2) / r;
        c = 1.0 / rr2;
        s = -c * r;
	}
	else
	{
		T r = x / y;
		T rr = r * r;
		T rr2 = std::sqrt(1.0 + rr);

		x = SIGN<T>(y, x) * rr;
		y = r - SIGN<T>(rr, r);
		s = SIGN<T>(1.0 / rr2, r);
		c = s * r;
	}
}

template <typename T, typename AT2>
void Kernel(AT2 matrix, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(int i = 0; i < size - 1; i++)
		for(int j = i + 1; j < size; j++)
		{
			T c = 0;
			T s = 0;

			Params<T>(matrix[i][i], matrix[j][i], c, s);

#pragma omp parallel for num_threads(LOC_THREADS)
			for(int k = i + 1; k < size; k++)
				Rot2d(c, s, matrix[i][k], matrix[j][k]);
		}

	LOC_PAPI_END_BLOCK
}

#endif
