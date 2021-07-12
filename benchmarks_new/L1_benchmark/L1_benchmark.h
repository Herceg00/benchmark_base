#include <string>

//#ifdef __USE_KUNPENG__
#include <arm_neon.h>
//#endif 

using std::string;

#define _16KB_ 16384 //bytes
#define _32KB_ 32768 //bytes
#define _256KB_ 256000 // bytes
#define _512KB_ 512000 // bytes
#define _7MB_ 7340032 //bytes

template<typename AT>
void Init(AT *a, AT **chunk, size_t size)
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
    for (int i = 0; i < omp_get_max_threads(); i++) {
        chunk[i] = a + i * chunk_size;
    }
}

template<typename AT>
float Kernel_read(AT *a, AT **chunk, size_t size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        unsigned int thread_num = myseed;
        float32x4_t null = {0, 0, 0, 0};
        float *chunk_start = chunk[thread_num];
        unsigned int offset = 0;
        float32x4_t local_sum1 = null;
        float32x4_t local_sum2 = null;
        float32x4_t local_sum3 = null;
        float32x4_t local_sum4 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            float32x4_t data1, data2, data3, data4;

            data1 = vld1q_f32(chunk_start + offset + 0);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 4);
            local_sum2 = vaddq_f32(local_sum2, data2);

            data3 = vld1q_f32(chunk_start + offset + 8);
            local_sum3 = vaddq_f32(local_sum3, data3);

            data4 = vld1q_f32(chunk_start + offset + 12);
            local_sum4 = vaddq_f32(local_sum4, data4);


            data1 = vld1q_f32(chunk_start + offset + 16);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 20);
            local_sum2 = vaddq_f32(local_sum2, data2);

            data3 = vld1q_f32(chunk_start + offset + 24);
            local_sum3 = vaddq_f32(local_sum3, data3);

            data4 = vld1q_f32(chunk_start + offset + 28);
            local_sum4 = vaddq_f32(local_sum4, data4);


            data1 = vld1q_f32(chunk_start + offset + 32);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 36);
            local_sum2 = vaddq_f32(local_sum2, data2);

            data3 = vld1q_f32(chunk_start + offset + 40);
            local_sum3 = vaddq_f32(local_sum3, data3);

            data4 = vld1q_f32(chunk_start + offset + 44);
            local_sum4 = vaddq_f32(local_sum4, data4);


            data1 = vld1q_f32(chunk_start + offset + 48);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 52);
            local_sum2 = vaddq_f32(local_sum2, data2);

            data3 = vld1q_f32(chunk_start + offset + 56);
            local_sum3 = vaddq_f32(local_sum3, data3);

            data4 = vld1q_f32(chunk_start + offset + 60);
            local_sum4 = vaddq_f32(local_sum4, data4);

            offset += 64;

            if ((offset + 64) > (_16KB_ / sizeof(float))) {
                offset = 0;
            }
        }
        float l_sum = 0;
        float temp[4];
        vst1q_f32(temp, local_sum1);
        for (int iter = 0; iter < 4; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum2);
        for (int iter = 0; iter < 4; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum3);
        for (int iter = 0; iter < 4; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum4);
        for (int iter = 0; iter < 4; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}

template<typename AT>
float Kernel(int core_type, AT *a, AT **chunk, size_t size)
{
    if(core_type == 0)
        return Kernel_read(a, chunk, size);
}
