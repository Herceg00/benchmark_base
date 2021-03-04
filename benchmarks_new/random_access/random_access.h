#include <string>

using std::string;

#define CACHE_LINE_K 256 // CACHE_LINE_K should be bigger than size of cache string


template<typename T, typename AT,typename IT, typename DT>
void InitSeq(AT a, IT b, DT c, int size)
{
#pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
#pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            a[i] = 0;
            b[i] = (int)rand_r(&myseed) % RADIUS;
            if(i < (int)RADIUS) {
                c[i] = rand_r(&myseed);
            }
        }
    }
}

template<typename T, typename AT,typename IT, typename DT>
void Init(AT a, IT b, DT c, int size)
{
    InitSeq<base_type, array_type, indirect_type, data_type>(a, b, c, size);
}

template<typename T, typename AT>
T Check(AT a, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
		sum += a[i] / size;

	return sum;
}

template<typename T, typename AT,typename IT, typename DT>
void Kernel(AT a, IT b, DT c, int size)
{
	LOC_PAPI_BEGIN_BLOCK

#pragma omp parallel for schedule(static)
    for(long int i = 0; i < size; i++)
    {
	    a[i] = c[(int)b[i]];
//	    printf("%d\n", b[i]);
//        a[i] = c[i] + b[i];
//        std::cout << b[i] << std::endl;
    }

	LOC_PAPI_END_BLOCK
}
