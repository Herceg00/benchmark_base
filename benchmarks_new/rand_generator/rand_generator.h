#include <string>

using std::string;

void Init()
{

}

template<typename AT>
long long Kernel_no_atomic(AT *a, size_t size)
{
    long long sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        sum += a[i];
    }
    return sum;
}

template<typename AT>
long long Kernel_atomic(AT *a, size_t size)
{
    long long sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        #pragma omp atomic
        sum += a[i];
    }
    return sum;
}

template<typename AT>
long long Kernel_critic(AT *a, size_t size)
{
    long long sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        #pragma omp critic
        sum += a[i];
    }
    return sum;
}

template<typename AT>
void Kernel(int core_type, AT *a, size_t size)
{
    switch (core_type)
    {
        long long sum;
        case  0:
            sum = Kernel_no_atomic(a, size);
            break;
        case  1:
            sum = Kernel_atomic(a, size);
            break;
        case  2:
            sum = Kernel_critic(a, size);
            break;
        default: fprintf(stderr, "Wrong core type of random generator!\n");
    }
}
