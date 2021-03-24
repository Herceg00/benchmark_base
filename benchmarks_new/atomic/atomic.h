#include <string>

using std::string;

typedef double base_type;

void Init()
{

}

template<typename AT>
base_type Kernel_no_atomic(AT *a, size_t size)
{
    base_type sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        sum += a[i];
    }
    return sum;
}

template<typename AT>
base_type Kernel_atomic(AT *a, size_t size)
{
    base_type sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        #pragma omp atomic
        sum += a[i];
    }
    return sum;
}

template<typename AT>
base_type Kernel_critical(AT *a, size_t size)
{
    base_type sum = 0;
    #pragma omp parallel for
    for (size_t i = 0; i < size; i++) {
        #pragma omp critical
        sum += a[i];
    }
    return sum;
}

template<typename AT>
void Kernel(int core_type, AT *a, size_t size)
{
    switch (core_type)
    {
        base_type sum;
        case  0:
            sum = Kernel_no_atomic(a, size);
            break;
        case  1:
            sum = Kernel_atomic(a, size);
            break;
        case  2:
            sum = Kernel_critical(a, size);
            break;
        default: fprintf(stderr, "Wrong core type of random generator!\n");
    }
}
