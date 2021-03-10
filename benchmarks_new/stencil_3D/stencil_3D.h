#include <string>

using std::string;

template<typename AT>
void InitSeq(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size*size*size; i++)
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
void Kernel_rectangle(AT *a, AT *b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t z = RADIUS; z < size - RADIUS; z++)
    {
        for(size_t y = RADIUS; y < size - RADIUS; y++)
        {
            for(size_t x = RADIUS; x < size - RADIUS; x++)
            {
                AT local_sum = 0;

                #pragma unroll(2*RADIUS+1)
                for (size_t z_s = z - RADIUS; z_s <= z + RADIUS; z_s++)
                    #pragma unroll(2*RADIUS+1)
                    for (size_t y_s = y - RADIUS; y_s <= y + RADIUS; y_s++)
                            #pragma unroll(2*RADIUS+1)
                            for (size_t x_s = x - RADIUS; x_s <= x + RADIUS; x_s++)
                                local_sum += b[z_s * size * size + y_s * size + x_s];

                a[z * size * size + y * size + x] = local_sum;
            }
        }
    }
}

template<typename AT>
void Kernel_cross(AT *a, AT *b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t z = RADIUS; z < size - RADIUS; z++)
    {
        for(size_t y = RADIUS; y < size - RADIUS; y++)
        {
            for(size_t x = RADIUS; x < size - RADIUS; x++)
            {
                AT local_sum = b[z*size*size + y * size + x];

                #pragma unroll(2*RADIUS+1)
                for (size_t y_s = y - RADIUS; y_s <= y + RADIUS; y_s++)
                    if(y_s != y)
                        local_sum += b[z*size*size + y_s * size + x];

                #pragma unroll(2*RADIUS+1)
                for (size_t x_s = x - RADIUS; x_s <= x + RADIUS; x_s++)
                    if(x_s != x)
                        local_sum += b[z*size*size + y * size + x_s];

                #pragma unroll(2*RADIUS+1)
                for (size_t z_s = z - RADIUS; z_s <= z + RADIUS; z_s++)
                    if(z_s != z)
                        local_sum += b[z_s*size*size + y * size + x];

                a[z*size*size + y * size + x] = local_sum;
            }
        }
    }
}

template <typename AT>
void Kernel(int core_type, AT *a, AT *b, size_t size)
{
    switch(core_type)
    {
        // rectangle
        case 0: Kernel_rectangle(a, b, size); break;

        // cross
        case 1: Kernel_cross(a, b, size); break;

        default: fprintf(stderr, "unexpected core type in stencil 2D");
    }
}
