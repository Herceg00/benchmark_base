#include <string>

#ifdef __USE_KUNPENG__
#include <arm_neon.h>
#endif 

//#ifdef __USE_INTEL__
#include <immintrin.h>
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

#ifdef __USE_KUNPENG__
template<typename AT>
float Kernel_read(AT *a, AT **chunk, size_t size)
{
    float return_val = 0;
    #pragma omp parallel
    {
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
#endif 

//#ifdef __USE_INTEL__
template<typename AT>
float Kernel_read(AT *a, AT **chunk, size_t size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        unsigned int thread_num = myseed;
        __m512 null = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        float *chunk_start = chunk[thread_num];
        unsigned int offset = 0;
        __m512 local_sum1 = null;
        __m512 local_sum2 = null;
        __m512 local_sum3 = null;
        __m512 local_sum4 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            __m512 data1, data2, data3, data4;

            data1 = _mm512_load_ps(chunk_start + offset + 16 * 0);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 1);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 2);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 3);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 4);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 5);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 6);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 7);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 8);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 9);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 10);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 11);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 12);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 13);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 14);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 15);
            local_sum4 = _mm512_add_ps(local_sum4, data4);

/*
            data1 = _mm512_load_ps(chunk_start + offset + 16 * 16);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 17);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 18);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 19);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 20);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 21);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 22);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 23);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 24);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 25);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 26);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 27);
            local_sum4 = _mm512_add_ps(local_sum4, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 28);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 29);
            local_sum2 = _mm512_add_ps(local_sum2, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 30);
            local_sum3 = _mm512_add_ps(local_sum3, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 31);
            local_sum4 = _mm512_add_ps(local_sum4, data4);
*/

            offset += 16 * 16;

            if ((offset + 16 * 16) > (_16KB_ / sizeof(float))) {
                offset = 0;
            }
        }
        
        float l_sum = 0;
        float *temp = (float *)malloc(sizeof(float) * 16);
        _mm512_store_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        
        _mm512_store_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_store_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_store_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
//#endif 


template<typename AT>
float Kernel(int core_type, AT *a, AT **chunk, size_t size)
{
    if(core_type == 0)
        return Kernel_read(a, chunk, size);
}
