#include <string>

using std::string;

template<typename AT>
void InitSeq(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size*size; i++)
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
    for(size_t y = RADIUS; y < size - RADIUS; y++)
    {
        for(size_t x = RADIUS; x < size - RADIUS; x++)
        {
            AT local_sum = 0;

            #pragma unroll(2*RADIUS+1)
            for (int y_s = y - RADIUS; y_s <= y + RADIUS; y_s++)
                #pragma unroll(2*RADIUS+1)
                for (int x_s = x - RADIUS; x_s <= x + RADIUS; x_s++)
                    local_sum += b[y_s * size + x_s];

            a[y * size + x] = local_sum;
        }
    }
}
