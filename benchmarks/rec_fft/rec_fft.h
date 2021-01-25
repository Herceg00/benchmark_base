/*----------------------------------------------------------------------------
	 fft.c - fast Fourier transform and its inverse (both recursively)
	 Copyright (C) 2004, Jerome R. Breitenbach.  All rights reserved.

	 The author gives permission to anyone to freely copy, distribute, and use
	 this file, under the following conditions:
			- No changes are made.
			- No direct commercial advantage is obtained.
			- No liability is attributed to the author for any damages incurred.
	----------------------------------------------------------------------------*/

/******************************************************************************
 * This file defines a C function fft that, by calling another function       *
 * fft_rec (also defined), calculates an FFT recursively.  Usage:             *
 *   fft(N, x, X);                                                            *
 * Parameters:                                                                *
 *   N: number of points in FFT (must equal 2^n for some integer n >= 1)      *
 *   x: pointer to N time-domain samples given in rectangular form (Re x,     *
 *      Im x)                                                                 *
 *   X: pointer to N frequency-domain samples calculated in rectangular form  *
 *      (Re X, Im X)                                                          *
 * Similarly, a function ifft with the same parameters is defined that        *
 * calculates an inverse FFT (IFFT) recursively.  Usage:                      *
 *   ifft(N, x, X);                                                           *
 * Here, N and X are given, and x is calculated.                              *
 ******************************************************************************/

#define TWO_PI (6.2831853071795864769252867665590057683943L)

template<typename T, typename AT>
void Init(AT x, AT X, AT XX, int size)
{
	for (int i = 0; i < size; i++)
	{
		x[i][0] = locality::utils::RRand(i) + 1;
		x[i][1] = locality::utils::RRand(locality::utils::RRand(i)) + 1;
		X[i][0] = 0;
		X[i][1] = 0;
		XX[i][0] = 0;
		XX[i][1] = 0;
	}
}

template<typename T, typename AT>
T Check(AT X, AT XX, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += (X[i][0] + XX[i][0]) / size + (X[i][1] + XX[i][1]) / size;

	return sum;
}

/* FFT recursion */
template<typename T, typename AT>
void Kernel(int offset, int delta,
	AT x, AT X, AT XX, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	int N2 = size/2;            /* half the number of points in FFT */
	int k;                   /* generic index */
	double cs, sn;           /* cosine and sine */
	int k00, k01, k10, k11;  /* indices for butterflies */
	double tmp0, tmp1;       /* temporary storage */

	if(size != 2)  /* Perform recursive step. */
		{
			/* Calculate two (size/2)-point DFT's. */
			Kernel<T, AT>(offset, 2*delta, x, XX, X, N2);
			Kernel<T, AT>(offset+delta, 2*delta, x, XX, X, N2);

			/* Combine the two (size/2)-point DFT's into one size-point DFT. */
			for(k=0; k<N2; k++)
				{
					k00 = offset + k*delta;    k01 = k00 + N2*delta;
					k10 = offset + 2*k*delta;  k11 = k10 + delta;
					//cs = cos(TWO_PI*k/(double)size); sn = sin(TWO_PI*k/(double)size);
					cs = 0.77; sn = 0.6; // we think that this functions are bad for actual results
					T k11_0 = XX[k11][0], k11_1 = XX[k11][1], k10_0 = XX[k10][0], k10_1 = XX[k10][1];
					tmp0 = cs * k11_0 + sn * k11_1;
					tmp1 = cs * k11_1 - sn * k11_0;
					X[k01][0] = k10_0 - tmp0;
					X[k01][1] = k10_1 - tmp1;
					X[k00][0] = k10_0 + tmp0;
					X[k00][1] = k10_1 + tmp1;
				}
		}
	else  /* Perform 2-point DFT. */
		{
			k00 = offset; k01 = k00 + delta;

			T k00_0 = x[k00][0], k00_1 = x[k00][1], k01_0 = x[k01][0], k01_1 = x[k01][1];

			X[k01][0] = k00_0 - k01_0;
			X[k01][1] = k00_1 - k01_1;
			X[k00][0] = k00_0 + k01_0;
			X[k00][1] = k00_1 + k01_1;
		}

	LOC_PAPI_END_BLOCK
}

/* IFFT */
template<typename T, typename AT>
void ifft(AT x, AT X, AT XX, int size)
{
	int N2 = size / 2;       /* half the number of points in IFFT */

	/* Calculate IFFT via reciprocity property of DFT. */
	fft_rec(0, 1, x, X, XX, size);

	x[0][0] = x[0][0] / size;
	x[0][1] = x[0][1] / size;
	x[N2][0] = x[N2][0] / size;
	x[N2][1] = x[N2][1] / size;

	for(int i = 1; i < N2; i++)
	{
		T tmp0 = x[i][0] / size;
		T tmp1 = x[i][1] / size;

		x[i][0] = x[size - i][0] / size;
		x[i][1] = x[size - i][1] / size;

		x[size - i][0] = tmp0;
		x[size - i][1] = tmp1;
	}
}
