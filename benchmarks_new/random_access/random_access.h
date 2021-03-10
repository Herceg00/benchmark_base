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
            a[i] = rand_r(&myseed);
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
void Kernel_load(AT *large, IT *indexes, DT *small, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t i = 0; i < size; i++)
    {
        large[i] = small[indexes[i]];
    }
}

template<typename AT,typename IT, typename DT>
void Kernel_store(AT *large, IT *indexes, DT *small, size_t size)
{
	#pragma omp parallel for schedule(static)
    for(size_t i = 0; i < size; i++)
    {
        small[indexes[i]] = large[i];
    }
}

template<typename AT,typename IT, typename DT>
void Kernel(int core_type, AT *large, IT *indexes, DT *small, size_t size)
{
    switch (core_type)
    {
        case  0:
            Kernel_load(large, indexes, small, size);
            break;
        case  1:
            Kernel_store(large, indexes, small, size);
            break;

        default: fprintf(stderr, "Wrong core type of random access!\n");
    }
}
