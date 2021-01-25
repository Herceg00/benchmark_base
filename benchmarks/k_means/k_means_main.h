#include <stdio.h>
#include <sys/time.h>

#include "locality.h"
#include "size.h"

static const size_t SIZE = SIZE_PREDEF;

static const size_t CLUSTERS = 8; // clusters
static const size_t DIM = 3; // dimension

static const double PREC = 1e-6;

typedef double base_type;
typedef base_type data_type[SIZE][DIM];
typedef base_type res_type[CLUSTERS][DIM];

typedef size_t labels_type[SIZE];
typedef size_t counts_type[CLUSTERS];

#include "k_means.h"

using namespace k_means;

double CallKernel(int core_type)
{
	static data_type data;
	static res_type centroids, tmp_centroids;
	static labels_type labels;
	static counts_type counts;

	timeval start, end;

	double time = -1;

	for(int i = 0; i < LOC_REPEAT; i++)
	{
		Init<data_type, res_type, labels_type, counts_type>(data, SIZE, DIM, CLUSTERS, centroids, tmp_centroids, labels, counts);
		locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);

		CallKernel<data_type, res_type, labels_type, counts_type>(core_type, data, SIZE, DIM, CLUSTERS, PREC, centroids, tmp_centroids, labels, counts);

gettimeofday(&end, NULL);

		printf("                  check_sum: %lg\n", Check<base_type, res_type>(centroids, CLUSTERS, DIM));

		double next_time = locality::utils::TimeDif(start, end);

		printf("                                      time: %lg\n", next_time);

		if(next_time < time || time < 0)
			time = next_time;
	}

	return time;
}

int main()
{
	LOC_PAPI_INIT

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

		locality::plain::Print(SIZE, time);
	}
}

