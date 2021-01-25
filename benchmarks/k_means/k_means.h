#ifndef K_MEANS
#define K_MEANS

#include <limits>
#include <cmath>

namespace k_means {

const int BENCH_COUNT = 1;

const char* type_names[BENCH_COUNT] = {
	"normal", // 0
};

template<typename AT2_D, typename AT2_C, typename AT_N, typename AT_K>
void Init(AT2_D data, size_t size, size_t dim, size_t clusters, AT2_C centroids, AT2_C tmp_centroids, AT_N labels, AT_K counts)
{
	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < dim; j++)
			data[i][j] = locality::utils::RRand(i, j);
	
	for(size_t i = 0; i < clusters; i++)
		for(size_t j = 0; j < dim; j++)
		{
			centroids[i][j] = 0;
			tmp_centroids[i][j] = 0;
		}

	for(size_t i = 0; i < clusters; i++)
		counts[i] = 0;

	for(size_t i = 0; i < size; i++)
		labels[i] = 0;
}

template <typename T, typename AT2_D>
T Check(AT2_D res, size_t size, size_t dim)
{
	T s = 0.0;

	for(size_t i = 0; i < size; i++)
		for(size_t j = 0; j < dim; j++)
			s += res[i][j];

	return s;
}

/*****
** kmeans.c
** - a simple k-means clustering routine
** - returns the cluster labels of the data points in an array
** - here's an example
**   extern int *k_means(double**, int, int, int, double, double**);
**   ...
**   int *c = k_means(data_points, num_points, dim, 20, 1e-4, 0);
**   for (i = 0; i < num_points; i++) {
**      printf("data point %d is in cluster %d\n", i, c[i]);
**   }
**   ...
**   free(c);
** Parameters
** - array of data points (double **data)
** - number of data points (int n)
** - dimension (int m)
** - desired number of clusters (int k)
** - error tolerance (double t)
**   - used as the stopping criterion, i.e. when the sum of
**     squared euclidean distance (standard error for k-means)
**     of an iteration is within the tolerable range from that
**     of the previous iteration, the clusters are considered
**     "stable", and the function returns
**   - a suggested value would be 0.0001
** - output address for the final centroids (double **centroids)
**   - user must make sure the memory is properly allocated, or
**     pass the null pointer if not interested in the centroids
** References
** - J. MacQueen, "Some methods for classification and analysis
**   of multivariate observations", Fifth Berkeley Symposium on
**   Math Statistics and Probability, 281-297, 1967.
** - I.S. Dhillon and D.S. Modha, "A data-clustering algorithm
**   on distributed memory multiprocessors",
**   Large-Scale Parallel Data Mining, 245-260, 1999.
** Notes
** - this function is provided as is with no warranty.
** - the author is not responsible for any damage caused
**   either directly or indirectly by using this function.
** - anybody is free to do whatever he/she wants with this
**   function as long as this header section is preserved.
** Created on 2005-04-12 by
** - Roger Zhang (rogerz@cs.dal.ca)
** Modifications
** -
** Last compiled under Linux with gcc-3
*/

template<typename AT2_D, typename AT2_C, typename AT_N, typename AT_K>
void k_means(AT2_D data, size_t size, size_t dim, size_t clusters, double prec, AT2_C centroids, AT2_C tmp_centroids, AT_N labels, AT_K counts)
{
	double old_error, error = std::numeric_limits<double>::max(); /* sum of squared euclidean distance */

	/****
	** initialization */

	for(size_t i = 0; i < clusters; i++)
	{
		/* pick k points as initial centroids */
		for(size_t j = dim; j > 0; )
		{
			j--;
			centroids[i][j] = data[i * size / clusters][j];
		}
	}

	/****
	** dimain loop */
	do {
		/* save error from last step */
		old_error = error, error = 0;

		/* clear old counts and temp centroids */
		for(size_t i = 0; i < clusters; i++)
		{
			counts[i] = 0;
			for(size_t j = 0; j < dim; j++)
			{
				tmp_centroids[i][j] = 0;
			}
		}

		for(size_t h = 0; h < size; h++)
		{
			/* identify the closest cluster */
			double min_distance = std::numeric_limits<double>::max();

			for(size_t i = 0; i < clusters; i++)
			{
				double distance = 0;

				for(size_t j = dim; j > 0; )
				{
					j--;
					distance += pow(data[h][j] - centroids[i][j], 2);
				}

				if(distance < min_distance)
				{
					labels[h] = i;
					min_distance = distance;
				}
			}
			/* update size and temp centroid of the destination cluster */
			for(size_t j = dim; j > 0; )
			{
				j--;
				tmp_centroids[labels[h]][j] += data[h][j];
			}

			counts[labels[h]]++;
			/* update standard error */
			error += min_distance;
		}

		for(size_t i = 0; i < clusters; i++)
		{ /* update all centroids */
			for(size_t j = 0; j < dim; j++)
			{
				centroids[i][j] = counts[i] ? tmp_centroids[i][j] / counts[i] : tmp_centroids[i][j];
			}
		}

	} while(fabs(error - old_error) > prec);
}

template<typename AT2_D, typename AT2_C, typename AT_N, typename AT_K>
void CallKernel(int core_type, AT2_D data, int size, int dim, int clusters, double prec, AT2_C centroids, AT2_C tmp_centroids, AT_N labels, AT_K counts)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		case 0: k_means(data, size, dim, clusters, prec, centroids, tmp_centroids, labels, counts); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

} /*namespace*/

#endif /*K_MEANS*/
