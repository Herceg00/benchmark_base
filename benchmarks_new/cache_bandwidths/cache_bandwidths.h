#include <string>
#include <sched.h>

using std::string;

#define STEPS_COUNT 1000

template<typename AT>
void Init(AT **private_caches_data, size_t size)
{
    #pragma omp parallel
    {
        unsigned int tid = omp_get_thread_num();
        AT *private_data = private_caches_data[tid];

        for(size_t i = 0; i < size; i++)
        {
            private_data[i] = rand_r(&tid);
        }
    }
}

double glob_accum;

template<typename AT>
void Kernel_private_cache(AT **data, size_t size)
{
    #pragma omp parallel shared(glob_accum)
    {
        unsigned int tid = omp_get_thread_num();
        AT *private_data = data[tid];

        AT accum = 0;
        #pragma unroll(STEPS_COUNT)
        for(size_t step = 0; step < STEPS_COUNT; step++)
        {
            for(size_t i = 0; i < size; i++)
            {
                accum += private_data[i] * step;
            }
            accum /= size;
        }

        glob_accum = accum;
    }
}

template<typename AT>
void Kernel(int core_type, AT **private_cache, size_t size)
{
    if(core_type == 0)
        Kernel_private_cache(private_cache, size);
}
