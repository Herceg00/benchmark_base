#include <string>

using std::string;

void Init()
{

}

template<typename AT>
void Kernel_reduction(AT *a, size_t size)
{
	AT k = a[0];
	AT sum = 0;
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static) reduction(+: sum)
        for (size_t i = 0; i < size; i++)
        {
           sum += rand_r(&myseed) - k;
        }
    }
}

template<typename AT>
void Kernel_storage(AT *a, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++)
        {
            a[i] = rand_r(&myseed);
        }
    }
}

template<typename AT>
void Kernel(int core_type, AT *a, size_t size)
{
    switch (core_type)
    {
        case  0:
            Kernel_reduction(a, size);
            break;
        case  1:
            Kernel_storage(a, size);
            break;

        default: fprintf(stderr, "Wrong core type of random generator!\n");
    }
}
