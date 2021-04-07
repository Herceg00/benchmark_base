#include <string>
#include <ctime>

using std::string;

typedef double base_type;

template<typename AT>
void Init(AT *x, int N)
{
    srand(time(0));
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        x[i] = double (rand() % 1000000000) / double (rand() % 1000000);
    }
}

template<typename AT>
void prefixsum_inplace(AT *x, int N) {
    AT *suma;
    #pragma omp parallel
    {
        const int ithread = omp_get_thread_num();
        const int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            suma = new AT[nthreads+1];
            suma[0] = 0;
        }
        AT sum = 0;
        #pragma omp parallel for schedule(static)
        for (int i=0; i<N; i++) {
            sum += x[i];
            x[i] = sum;
        }
        suma[ithread+1] = sum;
        #pragma omp barrier
        AT offset = 0;
        for(int i=0; i<(ithread+1); i++) {
            offset += suma[i];
        }
        #pragma omp parallel for schedule(static)
        for (int i=0; i<N; i++) {
            x[i] += offset;
        }
    }
    delete[] suma;
}

template<typename AT>
void Kernel(int core_type, AT *x, size_t size)
{
    switch (core_type)
    {
        base_type sum;
        case 0:
            prefixsum_inplace(x, size);
            break;
        default: fprintf(stderr, "Wrong core type of random generator!\n");
    }
}
