#include <string>

#ifdef __USE_KUNPENG__
#include <arm_neon.h>
#endif

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
    #pragma novector
    #pragma omp parallel for schedule(static)
    for(int y = RADIUS; y < size - RADIUS; y++)
    {
        #pragma simd
        #pragma ivdep
        #pragma vector
        for(int x = RADIUS; x < size - RADIUS; x++)
        {
            AT local_sum = 0;

            #pragma novector
            #pragma unroll(2*RADIUS+1)
            for (int y_s = -RADIUS; y_s <= RADIUS; y_s++)
                #pragma novector
                #pragma unroll(2*RADIUS+1)
                for (int x_s = -RADIUS; x_s <= RADIUS; x_s++)
                    local_sum += b[(y + y_s) * size + (x + x_s)];

            a[y * size + x] = local_sum;
        }
    }
}

#define RANGE (2*RADIUS + 1)

template <typename AT>
void Kernel_rectangle_kunpeng_optimized(AT * __restrict__ a, const AT * __restrict__ b, size_t size)
{
    throw "Error";
}

#ifdef __USE_KUNPENG__
template <>
void Kernel_rectangle_kunpeng_optimized<float>(float * __restrict__ a, const float * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        float32x4_t null = {0, 0, 0, 0};
        for (size_t x = RADIUS; x < LENGTH - RADIUS; x += 4)
        {
            float32x4_t local_sums = null;

            size_t offset = (y - RADIUS) * size + (x - RADIUS);

            #pragma unroll(RANGE)
            for (int y_s = 0; y_s < RANGE; y_s++) {
                #pragma unroll(RANGE)
                for (int x_s = 0; x_s < RANGE; x_s++) {
                    float32x4_t b_data = vld1q_f32(&b[offset + LENGTH * y_s + x_s]);
                    local_sums = vaddq_f32(local_sums, b_data);
                }
            }
            vst1q_f32(&a[y * size + x], local_sums);
        }
    }
}
#endif

#ifdef __USE_KUNPENG__
template <>
void Kernel_rectangle_kunpeng_optimized<double>(double * __restrict__ a, const double * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        float64x2_t null = {0, 0};
        for (size_t x = RADIUS; x < LENGTH - RADIUS; x += 2)
        {
            float64x2_t local_sums = null;

            size_t offset = (y - RADIUS) * size + (x - RADIUS);
            #pragma unroll(RANGE)
            for (int y_s = 0; y_s < RANGE; y_s++) {
                #pragma unroll(RANGE)
                for (int x_s = 0; x_s < RANGE; x_s++) {
                    float64x2_t b_data = vld1q_f64(&b[offset + LENGTH * y_s + x_s]);
                    local_sums = vaddq_f64(local_sums, b_data);
                }
            }
            vst1q_f64(&a[y * size + x], local_sums);
        }
    }
}
#endif

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

template <typename AT>
void Kernel_cross_kunpeng_optimized(AT * __restrict__ a, const AT * __restrict__ b, size_t size)
{
    throw "Error";
}

#ifdef __USE_KUNPENG__
template <>
void Kernel_cross_kunpeng_optimized<float>(float * __restrict__ a, const float * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        float32x4_t null = {0, 0, 0, 0};
        for (size_t x = RADIUS; x < LENGTH - RADIUS; x += 4)
        {
            float32x4_t local_sums = null;
            size_t offset = (y - RADIUS) * size + (x - RADIUS);

            #pragma unroll(RANGE)
            for (int y_s = 0; y_s < RANGE; y_s++)
            {
                float32x4_t b_data = vld1q_f32(&b[offset + LENGTH * y_s + RADIUS]);
                local_sums = vaddq_f32(local_sums, b_data);
            }

            #pragma unroll(RANGE)
            for (int x_s = 0; x_s < RANGE; x_s++)
            {
                float32x4_t b_data = vld1q_f32(&b[offset + LENGTH * RADIUS + x_s]);
                local_sums = vaddq_f32(local_sums, b_data);
            }

            float32x4_t b_data = vld1q_f32(&b[offset + LENGTH * RADIUS + RADIUS]);
            local_sums = vsubq_f32(local_sums, b_data);

            vst1q_f32(&a[y * size + x], local_sums);
        }
    }
}
#endif

#ifdef __USE_KUNPENG__
template <>
void Kernel_cross_kunpeng_optimized<double>(double * __restrict__ a, const double * __restrict__ b, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t y = RADIUS; y < LENGTH - RADIUS; y++)
    {
        float64x2_t null = {0, 0};
        for (size_t x = RADIUS; x < LENGTH - RADIUS; x += 2)
        {
            float64x2_t local_sums = null;
            size_t offset = (y - RADIUS) * size + (x - RADIUS);

            #pragma unroll(RANGE)
            for (int y_s = 0; y_s < RANGE; y_s++)
            {
                float64x2_t b_data = vld1q_f64(&b[offset + LENGTH * y_s + RADIUS]);
                local_sums = vaddq_f64(local_sums, b_data);
            }

            #pragma unroll(RANGE)
            for (int x_s = 0; x_s < RANGE; x_s++)
            {
                float64x2_t b_data = vld1q_f64(&b[offset + LENGTH * RADIUS + x_s]);
                local_sums = vaddq_f64(local_sums, b_data);
            }

            float64x2_t b_data = vld1q_f64(&b[offset + LENGTH * RADIUS + RADIUS]);
            local_sums = vsubq_f64(local_sums, b_data);

            vst1q_f64(&a[y * size + x], local_sums);
        }
    }
}
#endif

template <typename AT>
void Kernel(int core_type, AT *a, AT *b, size_t size)
{
    switch(core_type)
    {
        // rectangle
        case 0:
            Kernel_rectangle(a, b, size);
            break;

        // cross
        case 1:
            Kernel_cross(a, b, size);
            break;

            // rectangle
        case 2:
            //#ifdef __USE_KUNPENG__
            Kernel_rectangle_kunpeng_optimized(a, b, size);
            //#else
            //Kernel_rectangle(a, b, size);
            //#endif
            break;

            // cross
        case 3:
            //#ifdef __USE_KUNPENG__
            Kernel_cross_kunpeng_optimized(a, b, size);
            //#else
            //Kernel_cross(a, b, size);
            //#endif
            break;

        default: fprintf(stderr, "unexpected core type in stencil 2D");
    }
}
