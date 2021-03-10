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
void Kernel(AT *a, AT *b, size_t size)
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
