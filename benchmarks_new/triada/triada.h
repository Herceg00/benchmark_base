#include <string>
#include <sched.h>

using std::string;

#define CACHE_LINE_K 256 // CACHE_LINE_K should be bigger than size of cache string

const int CORE_TYPES = 16;

/** starting from 1 */
string GetCoreName(int core_type)
{
	string type_names[CORE_TYPES] =
	{
		"A[i] = B[i]*X + C", // 2
		"A[i] = B[i]*X[i] + C", // 3
		"A[i] = B[i]*X + C[i]", // 3
		"A[i] = B[i]*X[i] + C[i]", // 4
		"A[ind[i]] = B[ind[i]]*X + C",  // 3
		"A[ind[i]] = B[ind[i]]*X[ind[i]] + C",  // 4
		"A[ind[i]] = B[ind[i]]*X + C[ind[i]]", // 4
		"A[ind[i]] = B[ind[i]]*X[ind[i]]] + C[ind[i]]", // 5
		"A[ind[i]] = B[ind[i]]*X + C",  // 3
		"A[ind[i]] = B[ind[i]]*X[ind[i]] + C", // 4
		"A[ind[i]] = B[ind[i]]*X + C[ind[i]]", // 4
		"A[ind[i]] = B[ind[i]]*X[ind[i]]] + C[ind[i]]", // 5
		"A[i] = B[i] * 1 + 0", // 2
		"A[i] = B[i] * X + 0", // 2
		"A[i] = B[i] * 1 + C[i]", // 3
		"A[i] = B[i] * X + C[i]" // 4
	};

	if(core_type < 1 || core_type > CORE_TYPES)
		return "** ERROR **: bad kernel number";

	return type_names[core_type];
}

template<typename T, typename AT, typename AT_ind>
void InitSeq(AT a, AT b, AT c, AT x, AT_ind ind,int size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            ind[i] = i;
            a[i] = 0;
            b[i] = rand_r(&myseed);
            c[i] = rand_r(&myseed);
            x[i] = rand_r(&myseed);
        }
    }
}

template<typename T, typename AT, typename AT_ind>
void InitRand(AT a, AT b, AT c, AT x, AT_ind ind, int size)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();

        #pragma omp for schedule(static)
        for (int i = 0; i < size; i++) {
            ind[i] = (((long)i*CACHE_LINE_K)%size + ((long)i*CACHE_LINE_K/size))%size; // we try to make as much cache-misses as we can

            a[i] = 0;
            b[i] = rand_r(&myseed);
            c[i] = rand_r(&myseed);
            x[i] = rand_r(&myseed);
        }
    }
}

template<typename T, typename AT, typename AT_ind>
void Init(int core_type, AT a, AT b, AT c, AT x, AT_ind ind, int size)
{
	if(core_type < 9)
		InitSeq<T, AT, AT_ind>(a, b, c, x, ind ,size);
	else
		InitRand<T, AT, AT_ind>(a, b, c, x, ind, size);
}

template<typename T, typename AT>
T Check(AT a, int size)
{
	T sum = 0;
    #pragma omp parallel for schedule(static) reduction(+: sum)
	for(int i = 0; i < size; i++)
		sum += a[i] / size;

	return sum;
}

#define VAR(ASSIGN, _A, _B, _X, _C) ASSIGN; _A = _B * _X + _C;

#define CALL_AND_PROFILE(...) \
		for(long int i = 0; i < size; i++) \
		{ \
			VAR(__VA_ARGS__); \
		}

//	printf(#__VA_ARGS__);
//	printf("");

template<typename T, typename AT, typename AT_ind>
void Kernel(int core_type, AT a, AT b, AT c, AT x, AT_ind ind, int size)
{
	LOC_PAPI_BEGIN_BLOCK

	T sc_x = x[0], sc_c = c[0];

	switch (core_type) {
		case  0:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index =      i, a[index], b[index], sc_x    , sc_c)
		break;
		case  1:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index =      i, a[index], b[index], x[index], sc_c)
		break;
		case  2:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index =      i, a[index], b[index], sc_x    , c[index])
		break;
		case  3:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index =      i, a[index], b[index], x[index], c[index])
		break;
		case  4:
		case  8:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = ind[i], a[index], b[index], sc_x    , sc_c)
		break;
		case  5:
		case 9:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = ind[i], a[index], b[index], x[index], sc_c)
		break;
		case 6:
		case 10:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = ind[i], a[index], b[index], sc_x    , c[index])
		break;
		case  7:
		case 11:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = ind[i], a[index], b[index], x[index], c[index])
		break;

		case 12:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = i, a[index], b[index], 1, 0)
		break;
		case 13:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = i, a[index], b[index], sc_x, 0)
		break;
		case 14:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = i, a[index], b[index], 1, c[index])
		break;
		case 15:
            #pragma omp parallel for schedule(static)
			CALL_AND_PROFILE(long int index = i, a[index], b[index], sc_x, c[index])
		break;

		default: fprintf(stderr, "Wrong core type of triad!\n");
	}

	LOC_PAPI_END_BLOCK
}
