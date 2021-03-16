#include <string>
#include <sched.h>

using std::string;

#define STEPS_COUNT 1000

template<typename AT>
void Init(AT *data, size_t size)
{
    #pragma omp parallel
    {
        unsigned int tid = omp_get_thread_num();
        #pragma omp parallel for
        for(size_t i = 0; i < size; i++)
        {
            data[i] = rand_r(&tid);
        }
    }
}

template<typename AT>
void KERNEL(size_t size, size_t trials, AT * __restrict__ A)
{
    double alpha = 0.5;
    uint64_t i, j;
    for (j = 0; j < trials; ++j)
    {
        for (i = 0; i < size; ++i)
        {
            A[i] = A[i] + alpha;
        }
        alpha = alpha * 0.5;
    }
}

#include <limits>

using namespace std;

template<typename AT>
void Kernel(AT *data, size_t size)
{
    size_t TRIALS = 64;
    double global_min_bw = 0;
    int max_threads = omp_get_max_threads();
    #pragma omp parallel shared(global_min_bw)
    {
        unsigned int tid = omp_get_thread_num();
        AT *private_data = &(data[tid*size]);
        for (size_t n = 16; n < size; n *= 1.1)
        {
            global_min_bw = numeric_limits<double>::max();

            #pragma omp barrier
            double total_time = 0;
            size_t bytes_requested = 0;
            for (size_t t = 1; t < TRIALS; t *= 2)
            {
                double t1 = omp_get_wtime();
                KERNEL(n, t, private_data);
                double t2 = omp_get_wtime();
                bytes_requested += n * t * sizeof(AT);
                total_time += t2 - t1;
                #pragma omp barrier
            }
            #pragma omp barrier

            double cur_bw = bytes_requested/(total_time*1e9);
            #pragma omp critical
            {
                if(cur_bw < global_min_bw)
                    global_min_bw = cur_bw;
            };

            #pragma omp barrier
            #pragma omp master
            {
                std::cout << "SIZE = " << sizeof(AT)*n/(1024) << "KB " << " time: " << total_time << " BW: " << global_min_bw*max_threads << " GB/s" << std::endl;
            };
            #pragma omp barrier
        }
    }
}
