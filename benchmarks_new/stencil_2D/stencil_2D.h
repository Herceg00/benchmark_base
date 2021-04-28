#include <string>

#include <arm_neon.h>

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
    AT * __restrict__ a_local = a;
    const AT * __restrict__ b_local = b;

    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        float32x4_t null = {0, 0, 0, 0};

        for(size_t x = RADIUS; x < LENGTH - RADIUS; x += 4)
        {
            size_t offset = (y - 1) * size;

            float32x4_t local_sums = null;

            float32x4_t b_data = vld1q_f32(&b_local[offset]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + 1]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + 2]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH + 1]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH + 2]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH + LENGTH]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH + LENGTH + 1]);
            local_sums = vaddq_f32(local_sums, b_data);

            b_data = vld1q_f32(&b_local[offset + LENGTH + LENGTH + 2]);
            local_sums = vaddq_f32(local_sums, b_data);

            vst1q_f32(&a_local[offset + LENGTH + 1], local_sums);

            /*for(int i = 0; i < 4; i++)
            {
                float local_sum = 0;
                for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                    for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                        local_sum += b[(y + y_s) * size + (x + x_s + i)];
                if(local_sum != a_local[y * size + x + i])
                    #pragma omp critical
                    {
                        std::cout << a_local[y * size + x + i] << " " << local_sum << std::endl;
                    }
            }*/
        }
    }

    /*#pragma omp parallel for schedule(static)
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
    }*/
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
