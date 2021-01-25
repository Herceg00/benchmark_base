#ifndef QUBIT_EVOLUTION_H
#define QUBIT_EVOLUTION_H

#include <complex>

#define U_MAT_DIM 2

template <typename AT1, typename AT2>
void Init(AT1 in, AT1 out, AT2 u, int n_qubits)
{
	int size = 1 << n_qubits;

	for(int i = 0; i < size; i++)
	{
		in[i] = std::complex<double>(locality::utils::RRand(2*i), locality::utils::RRand(2*i + 1));
		out[i] = std::complex<double>();
	}

	for(int i = 0; i < U_MAT_DIM; i++)
		for(int j = 0; j < U_MAT_DIM; j++)
			u[i][j] = std::complex<double>(locality::utils::RRand(i, j), locality::utils::RRand(U_MAT_DIM + j, U_MAT_DIM + i));
}

template <typename AT1>
double Check(AT1 out, int n_qubits)
{
	int size = 1 << n_qubits;

	double sum = 0;

	for(int i = 0; i < size; i++)
		sum += out[i].real() + out[i].imag();

	return sum;
}

template <typename AT1, typename AT2>
void Kernel1(AT1 in, AT1 out, AT2 u, int n_qubits, int q)
{
	LOC_PAPI_BEGIN_BLOCK

	int size = 1 << n_qubits;

	int shift = n_qubits - q;
	int pow2q = 1 << shift;

	for(int i = 0; i < size; i++)
	{
		int i0 = i & ~pow2q;
 		int i1 = i | pow2q;
 		int iq = (i & pow2q) >> shift;

		out[i] = u[iq][0] * in[i0] + u[iq][1] * in[i1];
	}

	LOC_PAPI_END_BLOCK
}

#endif
