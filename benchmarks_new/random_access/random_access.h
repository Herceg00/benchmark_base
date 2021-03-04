#include <string>

using std::string;

#define CACHE_LINE_K 256 // CACHE_LINE_K should be bigger than size of cache string


template<typename T, typename AT,typename IT, typename AT_ind>
void InitSeq(AT a, AT b, IT c, int size)
{
#pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
#pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            a[i] = rand_r(&myseed);
            b[i] = rand_r(&myseed) % RADIUS;
            c[i] = rand_r(&myseed) ;
        }
    }
}

template<typename T, typename AT,typename IT, typename AT_ind>
void Init(AT a, AT b, IT c, int size)
{
    InitSeq<T, AT, IT, AT_ind>(a, b, c, size);
}

template<typename T, typename AT>
T Check(AT a, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += a[i] / size;

	return sum;
}

template<typename T, typename AT,typename IT, typename AT_ind>
void Kernel(AT a, AT b, IT c, int size)
{
	LOC_PAPI_BEGIN_BLOCK

#pragma omp parallel for schedule(static)
    for(long int i = 0; i < size; i++)
    {
	    a[i] = c[(int)b[i]];
    }

	LOC_PAPI_END_BLOCK
}
