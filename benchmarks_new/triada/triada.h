#include <string>
#include <sched.h>

using std::string;

#define CACHE_LINE_K RADIUS // CACHE_LINE_K should be bigger than size of cache string

const int CORE_TYPES = 10;

/*
"A[i] = B[ind[i]]*X + C", // 3
"A[ind[i]] = B[i]*X + C", // 3
"A[ind[i]] = B[ind[i]]*X + C", // 3

"A[i] = B[ind[i]]*X + C", // 3
"A[ind[i]] = B[i]*X + C", // 3
"A[ind[i]] = B[ind[i]]*X + C", // 3
*/

template<typename AT, typename AT_ind>
void InitSeq(AT *a, AT *b, AT *c, AT *x, AT_ind *ind, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < size; i++) {
            ind[i] = i;
            a[i] = rand_r(&myseed);
            b[i] = rand_r(&myseed);
            c[i] = rand_r(&myseed);
            x[i] = rand_r(&myseed);
        }
    }
}

template<typename AT, typename AT_ind>
void InitRand(AT *a, AT *b, AT *c, AT *x, AT_ind *ind, size_t size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();

        #pragma omp for schedule(static)
        for (size_t i = 0; i < size; i++) {
            ind[i] = (((size_t)i*CACHE_LINE_K)%size + ((size_t)i*CACHE_LINE_K/size))%size; // we try to make as much cache-misses as we can

            a[i] = 0;
            b[i] = rand_r(&myseed);
            c[i] = rand_r(&myseed);
            x[i] = rand_r(&myseed);
        }
    }
}

template<typename AT, typename AT_ind>
void Init(int core_type, AT *a, AT *b, AT *c, AT *x, AT_ind *ind, size_t size)
{
	if(core_type < 7)
		InitSeq(a, b, c, x, ind ,size);
	else
		InitRand(a, b, c, x, ind, size);
}

#define VAR(ASSIGN, _A, _B, _X, _C) ASSIGN; _A = _B * _X + _C;

#define CALL_AND_PROFILE(...) \
for(size_t i = 0; i < size; i++) \
{ \
    VAR(__VA_ARGS__); \
}

template<typename AT>
inline void tuned_Copy(AT *a, AT *c, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        c[j] = a[j];
}

template<typename AT>
inline void tuned_Scale(AT scalar, AT *c, AT *b, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        b[j] = scalar*c[j];
}

template<typename AT>
inline void tuned_Add(AT *a, AT *b, AT *c, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        c[j] = a[j]+b[j];
}

template<typename AT>
inline void tuned_Triad(AT scalar, AT *a, AT *b, AT *c, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        c[j] = a[j]+scalar*b[j];
}

template<typename AT, typename AT_ind>
inline void tuned_gather(AT *a, AT *b, AT_ind *ind, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        b[j] = a[ind[j]];
}

template<typename AT, typename AT_ind>
inline void tuned_scatter(AT *a, AT *b, AT_ind *ind, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        b[ind[j]] = a[j];
}

template<typename AT, typename AT_ind>
inline void tuned_scatter_gather(AT *a, AT *b, AT_ind *ind, size_t size)
{
    ssize_t j;
    #pragma omp parallel for
    for (j=0; j<size; j++)
        b[ind[j]] = a[ind[j]];
}


template<typename AT, typename AT_ind>
void Kernel(int core_type, AT *a, AT *b, AT *c, AT *x, AT_ind *ind, size_t size, AT scalar)
{
    base_type sc_x = x[0], sc_c = c[0];

	switch (core_type) {
	    case 0:
            tuned_Copy(a, c, size);
            break;
        case 1:
            tuned_Scale(scalar, b, c, size);
            break;
        case 2:
            tuned_Add(a, b, c, size);
            break;
        case 3:
            tuned_Triad(scalar, a, b, c, size);
            break;
		case  4:
            tuned_gather(a, b, ind, size);
		    break;
        case  5:
            tuned_scatter(a, b, ind, size);
            break;
        case  6:
            tuned_scatter_gather(a, b, ind, size);
            break;
        case  7:
            tuned_gather(a, b, ind, size);
            break;
        case  8:
            tuned_scatter(a, b, ind, size);
            break;
        case  9:
            tuned_scatter_gather(a, b, ind, size);
            break;

		default: fprintf(stderr, "Wrong core type of triad!\n");
	}
}
