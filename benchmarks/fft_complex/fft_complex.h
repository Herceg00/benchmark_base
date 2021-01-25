#ifndef FFT_COMPLEX_H
#define FFT_COMPLEX_H

#include <cmath>

namespace fft_complex {

template <typename T, typename AT>
void Init(AT x, AT pov, size_t size)
{
 	for(size_t i = 0; i < size; i++)
 	{
 		x[i * 2] = locality::utils::RRand(i * 2);
 		x[i * 2 + 1] = locality::utils::RRand(i * 2 + 1);
	}

 	for(size_t i = 0; i < size / 2; i++)
	{
 		pov[i * 2] = 0; // there should be some formula
 		pov[i * 2 + 1] = 0;
 	}
}

template <typename T, typename AT>
T Check(AT x, size_t size)
{
	T sum = 0;

	for (size_t i = 0; i < size; i++)
		sum += x[i];

	return sum / size;
}

#define C_MUL(TYPE, RES, RN, A, AN, B, BN) { TYPE _ar = A[(AN) * 2]; TYPE _ai = A[(AN) * 2 +1]; TYPE _br = B[(BN) * 2]; TYPE _bi = B[(BN) * 2 + 1]; RES[(RN) * 2] = _ar * _br - _ai * _bi; RES[(RN) * 2 + 1] = _ar * _bi + _br * _ai; }
#define C_ADD(RES, RN, A, AN, B, BN) { RES[(RN) * 2] = A[(AN) * 2] + B[(BN) * 2]; RES[(RN) * 2 + 1] = A[(AN) * 2 + 1] + B[(BN) * 2 + 1]; }
#define C_DIF(RES, RN, A, AN, B, BN) { RES[(RN) * 2] = A[(AN) * 2] - B[(BN) * 2]; RES[(RN) * 2 + 1] = A[(AN) * 2 + 1] - B[(BN) * 2 + 1]; }
#define C_MOV(RES, RN, A, AN) { RES[(RN) * 2] = A[(AN) * 2]; RES[(RN) * 2 + 1] = A[(AN) * 2 + 1]; }

template <typename T, typename AT>
void SubKernel(AT x, size_t i1, size_t i2, AT pov, size_t pov_n)
{
	T u[2];
	T v[2];
	T v2[2];

	C_ADD(u, 0, x, i1, x, i2);
	C_DIF(v2, 0, x, i1, x, i2);
	C_MUL(T, v, 0, v2, 0, pov, pov_n);

	C_MOV(x, i1, u, 0);
	C_MOV(x, i2, v, 0);
}

template <typename T, typename AT>
void FFT(AT x, AT pov, size_t size)
{
#pragma omp parallel for num_threads(LOC_THREADS)
	for(size_t i = 0; i < size / 2; i++)
	{
		pov[i * 2] = i * 2; // there should be some formula
		pov[i * 2 + 1] = i * 2 + 1;
	}

	for(size_t i = 0; i < std::log(size) / std::log(2.0); i++)
	{
		T p2i = std::pow(2.0, i);

#pragma omp parallel for num_threads(LOC_THREADS)
		for(int j = 0; j < (int)(size / 2 / p2i); j++)
		{
			for(int k = 0; k < (int)p2i; k++) // size_t.. stupid OpenMP!
			{
				SubKernel<T>(x, 2 * j * p2i + k, 2 * j * p2i + p2i + k, pov, j * p2i);
			}
		}
	}
}

template <typename AT>
void Conjugate(AT x, size_t size)
{
#pragma omp parallel for num_threads(LOC_THREADS)
	for(int i = 0; i < (int)size; i++) // size_t.. stupid OpenMP!
	{
		x[i * 2 + 1] = -x[i * 2 + 1];
	}
}

template <typename T, typename AT>
void IFFT(AT x, AT pov, size_t size)
{
	Conjugate<AT>(x, size);
	FFT<T, AT>(x, pov, size);
	Conjugate<AT>(x, size);
}

template <typename T, typename AT>
void Kernel(AT x, AT pov, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

	FFT<T, AT>(x, pov, size);

	LOC_PAPI_END_BLOCK
}

#undef C_MUL
#undef C_ADD
#undef C_DIF
#undef C_MOV

} /*namespace fft_complex*/

#endif /*FFT_COMPLEX_H*/
