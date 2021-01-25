#ifndef D_MATRIX_VECT_MULT
#define D_MATRIX_VECT_MULT

template <typename T, typename AT1, typename AT2>
void Init(AT1 vec_in, AT1 vec_out, AT2 matrix, int size)
{
	for(int i = 0; i < size; i++)
	{
		vec_in[i] = locality::utils::RRand(i);
		vec_out[i] = 0;

		for(int j = 0; j < size; j++)
			matrix[i][j] = locality::utils::RRand(i, j);
	}
}

template <typename T, typename AT1, typename AT2>
void Kernel(AT1 vec_in, AT1 vec_out, AT2 matrix, int size)
{
	LOC_PAPI_BEGIN_BLOCK

		for(int i = 0; i < size; i++)
			for(int j = 0; j < size; j++)
				vec_out[i] += matrix[i][j] * vec_in[j];

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT1>
double Check(AT1 vec_out, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += vec_out[i] / size;

	return sum;
}

#endif /*D_MATRIX_VECT_MULT*/
