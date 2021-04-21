#include <string>

using std::string;

template<typename AT>
void InitSeq(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++)
        {
            a[i] = rand_r(&myseed);
            b[i] = rand_r(&myseed);
        }
    }
}

template<typename AT>
void Init(AT *a, AT *b, size_t size)
{
    InitSeq(a, b, size);
}

template<typename AT>
void Kernel_base(AT *a, const AT *b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t i = RADIUS; i < size - RADIUS; i++)
    {
        AT local_sum = 0;
        #pragma unroll(2*RADIUS+1)
        for (size_t j = i - RADIUS; j < i + RADIUS + 1; j++)
        {
            local_sum += b[j];
        }
        a[i] = local_sum;
    }
}

template<typename AT>
void Kernel_restrict(AT * __restrict__ a, const AT * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t i = RADIUS; i < size - RADIUS; i++)
    {
        AT local_sum = 0;
        #pragma unroll(2*RADIUS+1)
        for (size_t j = i - RADIUS; j < i + RADIUS + 1; j++)
        {
            local_sum += b[j];
        }
        a[i] = local_sum;
    }
}

template<typename AT>
void Kernel(int core_type, AT *a, const AT *b, size_t size)
{
    if(core_type == 0)
        Kernel_base(a, b, size);
    if(core_type == 1)
        Kernel_restrict(a, b, size);
}
