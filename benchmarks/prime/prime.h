#ifndef PRIME
#define PRIME

#include <limits>

const int BENCH_COUNT = 1;

const char* type_names[BENCH_COUNT] = {
	"prime_1", // 0
};

template <typename T, typename AT>
void Init(AT array, size_t size)
{
	for(size_t i = 0; i < size; i++)
		array[i] = 0;
}

template<typename T, typename AT>
T Check(AT array, size_t size)
{
	T sum = 0;

	for(size_t i = 0; i < size; i++)
		sum += array[i];

	return sum;
}

template <typename T, typename AT>
void Kernel1(AT array, size_t size)
{
	LOC_PAPI_BEGIN_BLOCK

#pragma omp parallel for num_threads(LOC_THREADS)
	for(size_t i = 2; i < size; i++)
	{
		array[i] = 1;

		for(size_t j = 2; j < i; j++)
		{
			if(i % j == 0)
			{
				array[i] = 0;
				break;
			}
		}
	}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void CallKernel(int core_type, AT array, size_t size)
{
	switch(core_type)
	{
		case 0: Kernel1<T, AT>(array, size); break;

		default: fprintf(stderr, "unexpected core type");
	}
}

#endif /*PRIME*/
