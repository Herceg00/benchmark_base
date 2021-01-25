#ifndef MATRIX_TRANSP
#define MATRIX_TRANSP

const int BENCH_COUNT = 4;

const char* type_names[BENCH_COUNT] = {
	"ij", // 0
	"ji", // 1
	"ij_block", // 2
	"ji_block" // 3
};

template <typename T, typename AT>
void Init(AT a, AT b, int size)
{
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
		{
			a[i][j] = locality::utils::RRand(i, j);
			b[i][j] = 0.0;
		}
}

template <typename T, typename AT>
T Check(AT c, int size)
{
	T s = 0.0;

	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
			s += c[i][j] / size / size;

	return s;
}

#define LOOP(VAR) for(int VAR = 0; VAR < size; VAR ++)

#define OUTER_LOOP(VAR) for(int VAR = 0; VAR < size; VAR += block_size)
#define INNER_LOOP(VAR) for(int VAR##_b = VAR; VAR##_b < VAR + block_size; VAR##_b ++)

template <typename T, typename AT>
void KernelTranspIJ(AT a, AT b, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	LOOP(i)
		LOOP(j)
		{
			b[i][j] = a[j][i];
		}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void KernelTranspJI(AT a, AT b, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	LOOP(j)
		LOOP(i)
		{
			b[i][j] = a[j][i];
		}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void KernelBlockTranspIJ(AT a, AT b, int block_size, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	OUTER_LOOP(i)
		OUTER_LOOP(j)
			INNER_LOOP(i)
				INNER_LOOP(j)
				{
					b[i_b][j_b] = a[j_b][i_b];
				}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void KernelBlockTranspJI(AT a, AT b, int block_size, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	OUTER_LOOP(j)
		OUTER_LOOP(i)
			INNER_LOOP(j)
				INNER_LOOP(i)
				{
					b[i_b][j_b] = a[j_b][i_b];
				}

	LOC_PAPI_END_BLOCK
}


template <typename T, typename AT>
void CallKernel(int core_type, AT a, AT b, int block_size, int size)
{
	switch(core_type)
	{
		//ij
		case 0: KernelTranspIJ<T, AT>(a, b, size); break;

		//ji
		case 1: KernelTranspJI<T, AT>(a, b, size); break;

		//ij block
		case 2: KernelBlockTranspIJ<T, AT>(a, b, block_size, size); break;

		//ji block
		case 3: KernelBlockTranspJI<T, AT>(a, b, block_size, size); break;

		default: fprintf(stderr, "unexpected core type");
	}

}

#endif /*MATRIX_TRANSP*/
