#ifndef MATRIX_TRANSP
#define MATRIX_TRANSP

const int BENCH_COUNT = 4;

#include <omp.h>

const char* type_names[BENCH_COUNT] = {
	"ij", // 0
	"ji", // 1
	"ij_block", // 2
	"ji_block" // 3
};

template <typename AT>
void Init(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        unsigned int seed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for(size_t i = 0; i < size; i++)
        {
            for (size_t j = 0; j < size; j++)
            {
                b[i * size + j] = 0.0;
                a[i * size + j] = rand_r(&seed);
            }
        }
    }
}

#define LOOP(VAR) for(int VAR = 0; VAR < size; VAR ++)

#define OUTER_LOOP(VAR) for(int VAR = 0; VAR < size; VAR += block_size)
#define INNER_LOOP(VAR) for(int VAR##_b = VAR; VAR##_b < VAR + block_size; VAR##_b ++)

template <typename AT>
void KernelTranspIJ(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++)
        {
            for(size_t j = 0; j < size; j++)
            {
                b[i * size + j] = a[j * size + i]; // sequential writes (stores)
            }
        }
    }
}

template <typename AT>
void KernelTranspJI(AT *a, AT *b, size_t size)
{
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++)
        {
            for(size_t j = 0; j < size; j++)
            {
                b[j * size + i] = a[i * size + j]; // sequential reads (loads)
            }
        }
    }
}

template <typename AT>
void KernelBlockTranspIJ(AT *a, AT *b, size_t block_size, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t ii = 0; ii < size; ii += block_size)
    {
        for(size_t jj = 0; jj < size; jj += block_size)
        {
            for (size_t j = jj; j < jj+block_size; j++)
            {
                #pragma simd
                for (size_t i = ii; i < ii+block_size; i++)
                {
                    b[j*size + i] = a[i*size + j]; // sequential writes (stores)
                }
            }
        }
    }
}

template <typename AT>
void KernelBlockTranspJI(AT *a, AT *b, size_t block_size, size_t size)
{
    #pragma omp parallel for schedule(static)
    for(size_t ii = 0; ii < size; ii += block_size)
    {
        for(size_t jj = 0; jj < size; jj += block_size)
        {
            for (size_t j = jj; j < jj+block_size; j++)
            {
                #pragma simd
                for (size_t i = ii; i < ii+block_size; i++)
                {
                    b[j*size + i] = a[i*size + j]; // sequential reads (loads)
                }
            }
        }
    }
}


template <typename AT>
void Kernel(int core_type, AT *a, AT *b, size_t block_size, size_t size)
{
	switch(core_type)
	{
		//ij
		case 0: KernelTranspIJ(a, b, size); break;

		//ji
		case 1: KernelTranspJI(a, b, size); break;

		//ij block
		case 2: KernelBlockTranspIJ(a, b, block_size, size); break;

		//ji block
		case 3: KernelBlockTranspJI(a, b, block_size, size); break;

		default: fprintf(stderr, "unexpected core type in matrix transpose");
	}
}

#endif /*MATRIX_TRANSP*/
