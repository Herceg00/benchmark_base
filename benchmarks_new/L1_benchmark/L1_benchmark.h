#include <string>

//#ifdef __USE_KUNPENG__
#include <arm_neon.h>
//#endif 

using std::string;

#define _32KB_ 32768 //bytes
#define _7MB_ 7340032 //bytes

template<typename AT>
void Init(AT *a, AT **chunk, float32x4_t *local_sum, size_t size)
{
    #pragma omp parallel 
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule (static)
        for (size_t i = 0; i < size; i++) {
            a[i] = rand_r(&myseed);
        }
    }
    unsigned chunk_size = _7MB_ / sizeof(float);
    for (int i = 0; i < omp_get_num_threads(); i++) {
        chunk[i] = a + i * chunk_size;
        local_sum[i][0] = 0;
        local_sum[i][1] = 0;
        local_sum[i][2] = 0;
        local_sum[i][3] = 0;
    }
}

template<typename AT>
void Kernel_read(AT *a, AT **chunk, float32x4_t *local_sum, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        unsigned int thread_num = myseed;
        float *chunk_start = chunk[thread_num];
        unsigned int offset = 0;
        #pragma omp for schedule(static)
        for(int i = 0; i < RADIUS; i++)
        {
            float32x4_t data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            offset += 4;
            data = vld1q_f32(chunk_start + offset);
            local_sum[thread_num] = vaddq_f32(local_sum[thread_num], data);

            if (offset > _32KB_ / sizeof(float)) {
                offset = 0;
            }
        }
    }
}

template<typename AT>
void Kernel(int core_type, AT *a, AT **chunk, float32x4_t *local_sum, size_t size)
{
    if(core_type == 0)
        Kernel_read(a, chunk, local_sum, size);
}
