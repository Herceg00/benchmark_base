#include <string>

#ifdef __USE_KUNPENG__
#include <arm_neon.h>
#endif 

#ifdef __USE_INTEL__
#include <immintrin.h>
#endif 

using std::string;

#define _16KB_ 16384 //bytes
#define _32KB_ 32768 //bytes
#define _256KB_ 256000 // bytes
#define _512KB_ 512000 // bytes
#define _2MB_ 2097152 //bytes
#define _512KB_ 524288 //bytes

template<typename AT>
void Init(AT *a, AT *b, AT **chunk_read, AT **chunk_write, size_t size)
{
    #pragma omp parallel 
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule (static)
        for (size_t i = 0; i < size; i++) {
            a[i] = rand_r(&myseed);
        }
    }
    unsigned chunk_size = _2MB_ / sizeof(float);
    for (int i = 0; i < omp_get_max_threads(); i++) {
        chunk_read[i] = a + i * chunk_size;
        chunk_write[i] = b + i * chunk_size;
    }
}

#ifdef __USE_KUNPENG__
template<typename AT>
float Kernel_read(AT **chunk, size_t cache_size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
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
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum2);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum3);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum4);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 

#ifdef __USE_INTEL__
template<typename AT>
float Kernel_read(AT **chunk, size_t cache_size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
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

            offset += 16 * 16;

            if ((offset + 16 * 16) > (_16KB_ / sizeof(float))) {
                offset = 0;
            }
        }
        
        float l_sum = 0;
        float *temp = (float *)malloc(sizeof(float) * 16);
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 


#ifdef __USE_KUNPENG__
template<typename AT>
float Kernel_read_and_write(AT **chunk_read, AT **chunk_write, size_t cache_size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
        float32x4_t null = {0, 0, 0, 0};
        float *chunk_start_read = chunk_read[thread_num];
        float *chunk_start_write = chunk_write[thread_num];
        unsigned int offset = 0;
        float32x4_t local_sum1 = null;
        float32x4_t local_sum2 = null;
        float32x4_t local_sum3 = null;
        float32x4_t local_sum4 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            float32x4_t data1, data2, data3, data4;

            data1 = vld1q_f32(chunk_start_read + offset + 4 * 0);
            local_sum1 = vaddq_f32(local_sum1, data1);
            vst1q_f32(chunk_start_write + offset + 4 * 0, local_sum1);

            data2 = vld1q_f32(chunk_start_read + offset + 4);
            local_sum2 = vaddq_f32(local_sum2, data2);
            vst1q_f32(chunk_start_write + offset + 4 * 1, local_sum2);

            data3 = vld1q_f32(chunk_start_read + offset + 8);
            local_sum3 = vaddq_f32(local_sum3, data3);
            vst1q_f32(chunk_start_write + offset + 4 * 2, local_sum3);

            data4 = vld1q_f32(chunk_start_read + offset + 12);
            local_sum4 = vaddq_f32(local_sum4, data4);
            vst1q_f32(chunk_start_write + offset + 4 * 3, local_sum4);


            data1 = vld1q_f32(chunk_start_read + offset + 16);
            local_sum1 = vaddq_f32(local_sum1, data1);
            vst1q_f32(chunk_start_write + offset + 4 * 4, local_sum1);

            data2 = vld1q_f32(chunk_start_read + offset + 20);
            local_sum2 = vaddq_f32(local_sum2, data2);
            vst1q_f32(chunk_start_write + offset + 4 * 5, local_sum2);

            data3 = vld1q_f32(chunk_start_read + offset + 24);
            local_sum3 = vaddq_f32(local_sum3, data3);
            vst1q_f32(chunk_start_write + offset + 4 * 6, local_sum3);

            data4 = vld1q_f32(chunk_start_read + offset + 28);
            local_sum4 = vaddq_f32(local_sum4, data4);
            vst1q_f32(chunk_start_write + offset + 4 * 7, local_sum4);


            data1 = vld1q_f32(chunk_start_read + offset + 32);
            local_sum1 = vaddq_f32(local_sum1, data1);
            vst1q_f32(chunk_start_write + offset + 4 * 8, local_sum1);

            data2 = vld1q_f32(chunk_start_read + offset + 36);
            local_sum2 = vaddq_f32(local_sum2, data2);
            vst1q_f32(chunk_start_write + offset + 4 * 9, local_sum2);

            data3 = vld1q_f32(chunk_start_read + offset + 40);
            local_sum3 = vaddq_f32(local_sum3, data3);
            vst1q_f32(chunk_start_write + offset + 4 * 10, local_sum3);

            data4 = vld1q_f32(chunk_start_read + offset + 44);
            local_sum4 = vaddq_f32(local_sum4, data4);
            vst1q_f32(chunk_start_write + offset + 4 * 11, local_sum4);


            data1 = vld1q_f32(chunk_start_read + offset + 48);
            local_sum1 = vaddq_f32(local_sum1, data1);
            vst1q_f32(chunk_start_write + offset + 4 * 12, local_sum1);

            data2 = vld1q_f32(chunk_start_read + offset + 52);
            local_sum2 = vaddq_f32(local_sum2, data2);
            vst1q_f32(chunk_start_write + offset + 4 * 13, local_sum2);

            data3 = vld1q_f32(chunk_start_read + offset + 56);
            local_sum3 = vaddq_f32(local_sum3, data3);
            vst1q_f32(chunk_start_write + offset + 4 * 14, local_sum3);

            data4 = vld1q_f32(chunk_start_read + offset + 60);
            local_sum4 = vaddq_f32(local_sum4, data4);
            vst1q_f32(chunk_start_write + offset + 4 * 15, local_sum4);

            offset += 64;

            if ((offset + 64) > (cache_size / sizeof(float))) {
                offset = 0;
            }
        }
        float l_sum = 0;
        float temp[4];
        vst1q_f32(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum2);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum3);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        vst1q_f32(temp, local_sum4);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 

#ifdef __USE_INTEL__
template<typename AT>
float Kernel_read_and_write(AT **chunk_read, AT **chunk_write, size_t cache_size)
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
        __m512 null = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        float *chunk_start_read = chunk_read[thread_num];
        float *chunk_start_write = chunk_write[thread_num];
        unsigned int offset = 0;
        __m512 local_sum1 = null;
        __m512 local_sum2 = null;
        __m512 local_sum3 = null;
        __m512 local_sum4 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            __m512 data1, data2, data3, data4;

            data1 = _mm512_load_ps(chunk_start_read + offset + 16 * 0);
            local_sum1 = _mm512_add_ps(local_sum1, data1);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 0, local_sum1);

            data2 = _mm512_load_ps(chunk_start_read + offset + 16 * 1);
            local_sum2 = _mm512_add_ps(local_sum2, data2);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 1, local_sum2);

            data3 = _mm512_load_ps(chunk_start_read + offset + 16 * 2);
            local_sum3 = _mm512_add_ps(local_sum3, data3);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 2, local_sum3);

            data4 = _mm512_load_ps(chunk_start_read + offset + 16 * 3);
            local_sum4 = _mm512_add_ps(local_sum4, data4);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 3, local_sum4);


            data1 = _mm512_load_ps(chunk_start_read + offset + 16 * 4);
            local_sum1 = _mm512_add_ps(local_sum1, data1);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 4, local_sum1);

            data2 = _mm512_load_ps(chunk_start_read + offset + 16 * 5);
            local_sum2 = _mm512_add_ps(local_sum2, data2);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 5, local_sum2);

            data3 = _mm512_load_ps(chunk_start_read + offset + 16 * 6);
            local_sum3 = _mm512_add_ps(local_sum3, data3);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 6, local_sum3);

            data4 = _mm512_load_ps(chunk_start_read + offset + 16 * 7);
            local_sum4 = _mm512_add_ps(local_sum4, data4);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 7, local_sum4);


            data1 = _mm512_load_ps(chunk_start_read + offset + 16 * 8);
            local_sum1 = _mm512_add_ps(local_sum1, data1);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 8, local_sum1);

            data2 = _mm512_load_ps(chunk_start_read + offset + 16 * 9);
            local_sum2 = _mm512_add_ps(local_sum2, data2);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 9, local_sum2);

            data3 = _mm512_load_ps(chunk_start_read + offset + 16 * 10);
            local_sum3 = _mm512_add_ps(local_sum3, data3);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 10, local_sum3);

            data4 = _mm512_load_ps(chunk_start_read + offset + 16 * 11);
            local_sum4 = _mm512_add_ps(local_sum4, data4);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 11, local_sum4);


            data1 = _mm512_load_ps(chunk_start_read + offset + 16 * 12);
            local_sum1 = _mm512_add_ps(local_sum1, data1);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 12, local_sum1);

            data2 = _mm512_load_ps(chunk_start_read + offset + 16 * 13);
            local_sum2 = _mm512_add_ps(local_sum2, data2);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 13, local_sum2);

            data3 = _mm512_load_ps(chunk_start_read + offset + 16 * 14);
            local_sum3 = _mm512_add_ps(local_sum3, data3);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 14, local_sum3);

            data4 = _mm512_load_ps(chunk_start_read + offset + 16 * 15);
            local_sum4 = _mm512_add_ps(local_sum4, data4);
            _mm512_storeu_ps(chunk_start_write + offset + 16 * 15, local_sum4);

            offset += 16 * 16;

            if ((offset + 16 * 16) > (cache_size / sizeof(float))) {
                offset = 0;
            }
        }
        
        float l_sum = 0;
        float *temp = (float *)malloc(sizeof(float) * 16);
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 


#ifdef __USE_KUNPENG__
template<typename AT>
float Kernel_read_no_paral_instr(AT **chunk, size_t cache_size) // Без параллельных инструкций
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
        float32x4_t null = {0, 0, 0, 0};
        float *chunk_start = chunk[thread_num];
        unsigned int offset = 0;
        float32x4_t local_sum1 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            float32x4_t data1, data2, data3, data4;

            data1 = vld1q_f32(chunk_start + offset + 0);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 4);
            local_sum1 = vaddq_f32(local_sum1, data2);

            data3 = vld1q_f32(chunk_start + offset + 8);
            local_sum1 = vaddq_f32(local_sum1, data3);

            data4 = vld1q_f32(chunk_start + offset + 12);
            local_sum1 = vaddq_f32(local_sum1, data4);


            data1 = vld1q_f32(chunk_start + offset + 16);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 20);
            local_sum1 = vaddq_f32(local_sum1, data2);

            data3 = vld1q_f32(chunk_start + offset + 24);
            local_sum1 = vaddq_f32(local_sum1, data3);

            data4 = vld1q_f32(chunk_start + offset + 28);
            local_sum1 = vaddq_f32(local_sum1, data4);


            data1 = vld1q_f32(chunk_start + offset + 32);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 36);
            local_sum1 = vaddq_f32(local_sum1, data2);

            data3 = vld1q_f32(chunk_start + offset + 40);
            local_sum1 = vaddq_f32(local_sum1, data3);

            data4 = vld1q_f32(chunk_start + offset + 44);
            local_sum1 = vaddq_f32(local_sum1, data4);


            data1 = vld1q_f32(chunk_start + offset + 48);
            local_sum1 = vaddq_f32(local_sum1, data1);

            data2 = vld1q_f32(chunk_start + offset + 52);
            local_sum1 = vaddq_f32(local_sum1, data2);

            data3 = vld1q_f32(chunk_start + offset + 56);
            local_sum1 = vaddq_f32(local_sum1, data3);

            data4 = vld1q_f32(chunk_start + offset + 60);
            local_sum1 = vaddq_f32(local_sum1, data4);

            offset += 64;

            if ((offset + 64) > (cache_size / sizeof(float))) {
                offset = 0;
            }
        }
        float l_sum = 0;
        float temp[4];
        vst1q_f32(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 

#ifdef __USE_INTEL__
template<typename AT>
float Kernel_read_no_paral_instr(AT **chunk, size_t cache_size) // Без параллельных инструкций
{
    float return_val = 0;
    #pragma omp parallel
    {
        unsigned int thread_num = omp_get_thread_num();
        __m512 null = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        float *chunk_start = chunk[thread_num];
        unsigned int offset = 0;
        __m512 local_sum1 = null;
        for(int i = 0; i < RADIUS; i++)
        {
            __m512 data1, data2, data3, data4;

            data1 = _mm512_load_ps(chunk_start + offset + 16 * 0);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 1);
            local_sum1 = _mm512_add_ps(local_sum1, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 2);
            local_sum1 = _mm512_add_ps(local_sum1, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 3);
            local_sum1 = _mm512_add_ps(local_sum1, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 4);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 5);
            local_sum1 = _mm512_add_ps(local_sum1, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 6);
            local_sum1 = _mm512_add_ps(local_sum1, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 7);
            local_sum1 = _mm512_add_ps(local_sum1, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 8);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 9);
            local_sum1 = _mm512_add_ps(local_sum1, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 10);
            local_sum1 = _mm512_add_ps(local_sum1, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 11);
            local_sum1 = _mm512_add_ps(local_sum1, data4);


            data1 = _mm512_load_ps(chunk_start + offset + 16 * 12);
            local_sum1 = _mm512_add_ps(local_sum1, data1);

            data2 = _mm512_load_ps(chunk_start + offset + 16 * 13);
            local_sum1 = _mm512_add_ps(local_sum1, data2);

            data3 = _mm512_load_ps(chunk_start + offset + 16 * 14);
            local_sum1 = _mm512_add_ps(local_sum1, data3);

            data4 = _mm512_load_ps(chunk_start + offset + 16 * 15);
            local_sum1 = _mm512_add_ps(local_sum1, data4);

            offset += 16 * 16;

            if ((offset + 16 * 16) > (cache_size / sizeof(float))) {
                offset = 0;
            }
        }
        
        float l_sum = 0;
        float *temp = (float *)malloc(sizeof(float) * 16);
        _mm512_storeu_ps(temp, local_sum1);
        for (int iter = 0; iter < 16; iter++){
            l_sum += temp[iter];
        }
        return_val = l_sum;
    }
    return return_val;
}
#endif 

template<typename AT>
float Kernel(int core_type, AT **chunk_read, AT **chunk_write, size_t cache_size)
{
    if(core_type == 0)
        return Kernel_read(chunk_read, _32KB_);
    if(core_type == 1) 
        return Kernel_read_and_write(chunk_read, chunk_write, _32KB_);
    if(core_type == 2) 
        return Kernel_read_no_paral_instr(chunk_read, _32KB_);
    if(core_type == 3)
        return Kernel_read(chunk_read, _512KB_);
    if(core_type == 4) 
        return Kernel_read_and_write(chunk_read, chunk_write, _512KB_);
    if(core_type == 5) 
        return Kernel_read_no_paral_instr(chunk_read, _512KB_);
}
