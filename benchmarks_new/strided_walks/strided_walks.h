#include <string>
#include <sched.h>

using std::string;

template<typename AT>
void Init(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for
        for(size_t i = 0; i < size; i++)
        {
            a[i] = rand_r(&myseed);
            b[i] = rand_r(&myseed);
        }
    }
}

template<typename AT>
void kernel_parallel_reads(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        size_t src = omp_get_thread_num() * (size/omp_get_num_threads()) + omp_get_thread_num();
        #pragma omp for
        for(size_t i = 0; i < size; i++)
        {
            a[i] = b[src];
            src += STRIDE;
            if(src >= size)
                src = src % size;
        }
    }
}

template<typename AT>
void kernel_sequential_reads(AT *a, AT *b, size_t size)
{
    size_t src = 0;
    for(size_t i = 0; i < size; i++)
    {
        a[i] = b[src];
        src += STRIDE;
        if(src >= size)
            src = src % size;
    }
}

template<typename AT>
void kernel_parallel_writes(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        size_t dst = omp_get_thread_num() * (size/omp_get_num_threads()) + omp_get_thread_num();
        #pragma omp for
        for(size_t i = 0; i < size; i++)
        {
            a[dst] = b[i];
            dst += STRIDE;
            if(dst >= size)
                dst = dst % size;
        }
    }
}

template<typename AT>
void kernel_sequential_writes(AT *a, AT *b, size_t size)
{
    size_t dst = 0;
    for(size_t i = 0; i < size; i++)
    {
        a[dst] = b[i];
        dst += STRIDE;
        if(dst >= size)
            dst = dst % size;
    }
}

template<typename AT>
void Kernel(int core_type, AT *a, AT *b, size_t size)
{
    switch (core_type) {
        case 0:
            kernel_parallel_reads(a, b, size);
            break;
        case 1:
            kernel_parallel_writes(a, b, size);
            break;
        default: fprintf(stderr, "Wrong core type of triad!\n");
    }
}
