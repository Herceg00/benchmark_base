#include <string>
#include <sched.h>

using std::string;

template<typename AT>
void Init(AT *a, AT *b, AT *c, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < size; i++) {
            a[i] = rand_r(&myseed);
            b[i] = rand_r(&myseed);
            c[i] = rand_r(&myseed);
        }
    }
}

template<typename AT>
void Kernel(AT *a, AT *b, AT *c, size_t size, AT scalar, int threads)
{
    #pragma omp parallel for num_threads(threads)
    for (size_t i = 0; i < size; i++)
        a[i] = scalar * b[i] + c[i];
}
