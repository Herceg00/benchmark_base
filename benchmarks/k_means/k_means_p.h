#include <stdio.h>
#include <sys/time.h>

#include "locality.h"

static const size_t SIZE = SIZE_PREDEF;

static const size_t CLUSTERS = 8; // clusters
static const size_t DIM = 3; // dimension

static const double PREC = 1e-6;

typedef double base_type;
typedef locality::PtrWrapper<base_type, 2> data_type;
typedef locality::PtrWrapper<base_type, 2> res_type;

typedef locality::PtrWrapper<size_t, 1> labels_type;
typedef locality::PtrWrapper<size_t, 1> counts_type;

#include "k_means.h"

using namespace k_means;

double CallKernel(int core_type)
{
	static data_type data("data", DIM2(SIZE, DIM));

	static res_type centroids("centroids", DIM2(CLUSTERS, DIM));
	static res_type tmp_centroids("tmp_centroids", DIM2(CLUSTERS, DIM));

	static labels_type labels("labels", DIM1(SIZE));
	static counts_type counts("counts", DIM1(CLUSTERS));

	timeval start, end;

	Init<data_type, res_type, labels_type, counts_type>(data, SIZE, DIM, CLUSTERS, centroids, tmp_centroids, labels, counts);

	locality::utils::CacheAnnil(core_type);

gettimeofday(&start, NULL);
locality::profiler::Logging(true);

	CallKernel<data_type, res_type, labels_type, counts_type>(core_type, data, SIZE, DIM, CLUSTERS, PREC, centroids, tmp_centroids, labels, counts);

locality::profiler::Logging(false);
gettimeofday(&end, NULL);

	printf("check_sum: %lg\n", Check<base_type, res_type>(centroids, CLUSTERS, DIM));

	double time = locality::utils::TimeDif(start, end);

	return time;
}

extern "C" int main()
{
#ifdef LOC_LOG_PATH
	locality::memory_trace::Init(LOC_LOG_PATH);
#endif

#ifdef LOC_CVG_PARAMS
	locality::cvg::Init(LOC_CVG_PARAMS);
#endif

	for(int core_type = 0; core_type < BENCH_COUNT; core_type++)
	{
		locality::plain::Rotate(type_names[core_type]);

		double time = CallKernel(core_type);

#ifdef LOC_LOG_PATH
		locality::memory_trace::Print(SIZE, time);
#endif

#ifdef LOC_CVG_PARAMS
		locality::cvg::Print(SIZE, time);
#endif
	}
}

