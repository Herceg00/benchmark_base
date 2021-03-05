#include <string>

using std::string;

template<typename AT,typename IT, typename DT>
void InitSeq(AT *a, IT *b, DT *c, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++)
        {
            a[i] = 0;
            b[i] = (int)rand_r(&myseed) % RADIUS_IN_ELEMENTS;
        }

        #pragma omp for schedule(static)
        for (size_t i = 0; i < RADIUS_IN_ELEMENTS; i++)
        {
            c[i] = rand_r(&myseed);
        }
    }
}

template<typename AT,typename IT, typename DT>
void Init(AT *a, IT *b, DT *c, size_t size)
{
    InitSeq(a, b, c, size);
}

template<typename AT,typename IT, typename DT>
void Kernel(AT *a, IT *b, DT *c, size_t size)
{
	#pragma omp parallel for schedule(static)
    for(size_t i = 0; i < size; i++)
    {
	    a[i] = c[b[i]];
    }
}
