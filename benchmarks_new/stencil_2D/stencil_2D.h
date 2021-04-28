#include <string>

using std::string;

#define BLOCK_SIZE 16

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
void Kernel_rectangle(AT * __restrict__ a, const AT * __restrict__ b, const size_t size)
{
    #pragma omp parallel for schedule(static)
    for(int y = RADIUS; y < size - RADIUS; y++)
    {
        #pragma simd
        #pragma ivdep
        #pragma vector
        for(int x = RADIUS; x < size - RADIUS; x++)
        {
            AT local_sum = 0;

            #pragma unroll(2*RADIUS+1)
            for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                #pragma unroll(2*RADIUS+1)
                for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                    local_sum += b[(y + y_s) * size + (x + x_s)];

            a[y * size + x] = local_sum;
        }
    }
}

template<typename AT>
void Kernel_rectangle_blocked(AT * __restrict__ a, const AT * __restrict__ b, size_t size)
{
    /*AT * __restrict__ a_local = a;
    const AT * __restrict__ b_local = b;

    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        for(size_t x = RADIUS; x < LENGTH - RADIUS; x += 2)
        {
            AT reg_sum[2] = {0, 0};
            #pragma simd
            #pragma ivdep
            #pragma vector
            for(int i = 0; i < 2; i++)
            {
                reg_sum[i] += b_local[(y -1) * size + (x - 1 + i)];
                reg_sum[i] += b_local[(y -1) * size + x + i];
                reg_sum[i] += b_local[(y -1) * size + (x + 1 + i)];
                reg_sum[i] += b_local[y * size + (x - 1 + i)];
                reg_sum[i] += b_local[y * size + x + i];
                reg_sum[i] += b_local[y * size + (x + 1 + i)];
                reg_sum[i] += b_local[(y + 1) * size + (x - 1 + i)];
                reg_sum[i] += b_local[(y + 1) * size + x + i];
                reg_sum[i] += b_local[(y + 1) * size + (x + 1 + i)];

                a_local[y * size + x + i] = reg_sum[i];
            }
        }

        for(size_t x = RADIUS; x < LENGTH - RADIUS; x++)
        {
            AT local_sum = 0;

            local_sum += b_local[(y -1) * size + (x - 1)];
            local_sum += b_local[(y -1) * size + x];
            local_sum += b_local[(y -1) * size + (x + 1)];
            local_sum += b_local[y * size + (x - 1)];
            local_sum += b_local[y * size + x];
            local_sum += b_local[y * size + (x + 1)];
            local_sum += b_local[(y + 1) * size + (x -1 )];
            local_sum += b_local[(y + 1) * size + x];
            local_sum += b_local[(y + 1) * size + (x + 1)];

            a_local[y * size + x] = local_sum;
        }
    }*/

    #pragma omp parallel for schedule(static)
    for(size_t y_st = RADIUS; y_st < size - RADIUS; y_st += BLOCK_SIZE)
    {
        for(size_t x_st = RADIUS; x_st < size - RADIUS; x_st += BLOCK_SIZE)
        {
            for(size_t yy = 0; yy < BLOCK_SIZE; yy++)
            {
                #pragma simd
                #pragma ivdep
                #pragma vector
                for(size_t xx = 0; xx < BLOCK_SIZE; xx++)
                {
                    size_t x = x_st + xx;
                    size_t y = y_st + yy;

                    if((x < (size - RADIUS)) && y < ((size - RADIUS)))
                    {
                        AT local_sum = 0;
                        #pragma unroll(2*RADIUS+1)
                        for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                            #pragma unroll(2*RADIUS+1)
                            for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                                local_sum += b[(y + y_s) * size + (x + x_s)];

                        a[y * size + x] = local_sum;
                    }
                }
            }
        }
    }
}

template<typename AT>
void Kernel_cross(AT * __restrict__ a, AT * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(int y = RADIUS; y < size - RADIUS; y++)
    {
        #pragma simd
        #pragma ivdep
        #pragma vector
        for(int x = RADIUS; x < size - RADIUS; x++)
        {
            AT local_sum = b[y * size + x];

            #pragma unroll(2*RADIUS+1)
            for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                if(y_s != 0)
                    local_sum += b[(y + y_s) * size + x];

            #pragma unroll(2*RADIUS+1)
            for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                if(x_s != 0)
                    local_sum += b[y * size + x + x_s];

            a[y * size + x] = local_sum;
        }
    }
}

template<typename AT>
void Kernel_cross_blocked(AT * __restrict__ a, AT * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t y_st = RADIUS; y_st < size - RADIUS; y_st += BLOCK_SIZE)
    {
        for(size_t x_st = RADIUS; x_st < size - RADIUS; x_st += BLOCK_SIZE)
        {
            for(size_t yy = 0; yy < BLOCK_SIZE; yy++)
            {
                #pragma simd
                #pragma ivdep
                #pragma vector
                for(size_t xx = 0; xx < BLOCK_SIZE; xx++)
                {
                    size_t x = x_st + xx;
                    size_t y = y_st + yy;

                    if((x < (size - RADIUS)) && y < ((size - RADIUS)))
                    {
                        AT local_sum = b[y * size + x];

                        #pragma unroll(2*RADIUS+1)
                        for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                            if(y_s != 0)
                                local_sum += b[(y + y_s) * size + x];

                        #pragma unroll(2*RADIUS+1)
                        for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                            if(x_s != 0)
                                local_sum += b[y * size + x + x_s];

                        a[y * size + x] = local_sum;
                    }
                }
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

            // rectangle
        case 2: Kernel_rectangle_blocked(a, b, size); break;

            // cross
        case 3: Kernel_rectangle_blocked(a, b, size); break;

        default: fprintf(stderr, "unexpected core type in stencil 2D");
    }
}
