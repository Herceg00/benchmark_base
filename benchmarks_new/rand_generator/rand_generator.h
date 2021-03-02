#include <string>

using std::string;

#define CACHE_LINE_K 256 // CACHE_LINE_K should be bigger than size of cache string


template<typename T, typename AT, typename AT_ind>
void InitSeq()
{

}

template<typename T, typename AT, typename AT_ind>
void Init()
{
    InitSeq<T, AT, AT_ind>();
}

template<typename T, typename AT>
T Check(AT a, int size)
{
	T sum = 0;
#pragma omp parallel for
	for(int i = 0; i < size; i++)
		sum += a[i] / size;

	return sum;
}

template<typename T, typename AT, typename AT_ind>
void Kernel_reduction(AT a, int size)
{
	LOC_PAPI_BEGIN_BLOCK
	T k = a[0];
	T sum = 0;
#pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
#pragma omp for schedule(static) reduction(+: sum)
        for (int i = 0; i < size; i++) {
           sum += rand_r(&myseed) - k;
        }
    }

	LOC_PAPI_END_BLOCK
}

template<typename T, typename AT, typename AT_ind>
void Kernel_storage(AT a, int size)
{
    LOC_PAPI_BEGIN_BLOCK
#pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
#pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            a[i] = rand_r(&myseed);
        }
    }
    LOC_PAPI_END_BLOCK
}
