#include <string>

#ifdef __USE_KUNPENG__
#include <arm_neon.h>
#endif 

#ifdef __USE_INTEL__
#include <immintrin.h>
#endif 

using std::string;

#define SAME_SOCKET 0
#define DIFF_SOCKET 1

template<typename AT>
void Init(AT *a1, AT *b1, AT *c1, AT *a2, AT *b2, AT *c2, size_t size)
{
    #pragma omp parallel 
    {
        int thread_num = omp_get_thread_num();
        if (thread_num < 64) {
            unsigned int myseed = thread_num;
            #pragma omp for schedule (static)
            for (size_t i = 0; i < size; i++) {
                a1[i] = rand_r(&myseed);
                b1[i] = rand_r(&myseed);
                c1[i] = rand_r(&myseed);
            }
        } else {
            unsigned int myseed = thread_num;
            #pragma omp for schedule (static)
            for (size_t i = 0; i < size; i++) {
                a2[i] = rand_r(&myseed);
                b2[i] = rand_r(&myseed);
                c2[i] = rand_r(&myseed);
            }
        }
    }
}

template<typename AT>
inline void func(AT scalar, AT *a, AT *b, AT *c, size_t size)
{
    #pragma omp parallel
    {
        #pragma omp for
        for (size_t j=0; j<size; j++)
            c[j] = a[j]+scalar*b[j];
    }
}

template<typename AT>
float Kernel(int core_type, AT *a1, AT *b1, AT *c1, AT *a2, AT *b2, AT *c2, size_t size)
{
    AT scalar = 123.456;
    fprintf(stderr, "THE BEGINNING\n");
    if(core_type == SAME_SOCKET) {
   //     func(scalar, a1, b1, c1, size);
    }
    if(core_type == DIFF_SOCKET) {
     //   func(scalar, a2, b2, c2, size);
    }
    fprintf(stderr, "THE END\n");
}
