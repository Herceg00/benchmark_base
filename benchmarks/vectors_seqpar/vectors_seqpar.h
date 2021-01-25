#ifndef VECTORS_SEQPAR
#define VECTORS_SEQPAR

const int BENCH_COUNT = 3;

const char* type_names[BENCH_COUNT] = {
	"op_seqpar", // 0
	"scalar_seqpar", // 1
	"uniform_norm_seqpar" // 2
};

template <typename T, typename AT>
void Init(AT a, AT b, AT c, int size)
{
	for(int i = 0; i < size; i++)
	{
		a[i] = locality::utils::RRand(i);
		b[i] = locality::utils::RRand(i << 2);
		c[i] = 0;
	}
}

template <typename T, typename AT>
T Check(AT c, int)
{
	return c[0];
}

/**
	tmp is used as temporary array to store values from different processes
*/
template <typename T, typename AT>
void KernelOpSeqpar(AT a, AT tmp, AT c, int block_size, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	T sum = 0;

	for(int i = 0; i < size / block_size; i++)
	{
		T tmp_sum = 0;

		for(int j = 0; j < block_size; j++)
		{
			tmp_sum += a[i*block_size + j];
		}

		tmp[i] = tmp_sum;
	}

	for(int i = 0; i < size / block_size; i++)
		sum += tmp[i];

	c[0] = sum;

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void KernelScalarSeqpar(AT a, AT b, AT tmp, int block_size, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	T sum = 0;

	for(int i = 0; i < size / block_size; i++)
	{
		T tmp_sum = 0;

		for(int j = 0; j < block_size; j++)
		{
			tmp_sum += a[i*block_size + j] * b[i*block_size + j];
		}

		tmp[i] = tmp_sum;
	}

	for(int i = 0; i < size / block_size; i++)
		sum += tmp[i];

	tmp[0] = sum;

	LOC_PAPI_END_BLOCK
}

namespace vectors_seqpar
{
	template <typename T>
	T abs(T a)
	{
		return a > 0 ? a : -a;
	}

	template <typename T>
	T max(T a, T b)
	{
		return a > b ? a : b;
	}
}

template <typename T, typename AT>
void KernelUniformNormSeqpar(AT a, AT, AT tmp, int block_size, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	for(int i = 0; i < size / block_size; i++)
	{
		T tmp_max = a[i*block_size];

		for(int j = 1; j < block_size; j++)
		{
			tmp_max = vectors_seqpar::max(tmp_max, vectors_seqpar::abs(a[i*block_size + j]));
		}

		tmp[i] = tmp_max;
	}

	T max = tmp[0];

	for(int i = 1; i < size / block_size; i++)
		max = vectors_seqpar::max(max, tmp[i]);

	tmp[0] = max;

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void CallKernel(int core_type, AT a, AT b, AT c, int block_size, int size)
{
	switch(core_type)
	{
		//op_seqpar
		case 0: KernelOpSeqpar<T, AT>(a, b, c, block_size, size); break;

		//scalar_seqpar
		case 1: KernelScalarSeqpar<T, AT>(a, b, c, block_size, size); break;

		//unifor_norm_seqpar
		case 2: KernelUniformNormSeqpar<T, AT>(a, b, c, block_size, size); break;

		default: fprintf(stderr, "unexpected core type");
	}
}

#endif /*VECTORS_SEQPAR*/
