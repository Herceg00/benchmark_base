#ifndef DITHERING
#define DITHERING

#include <cmath>
#include <iostream>

const int BENCH_COUNT = 2;

const char* type_names[BENCH_COUNT] = {
	"normal", // 0
	"skewed", // 1
};

template <typename T, typename AT>
void Init(AT src, AT dst, int size)
{
	for(int i = 0; i < size; i++)
		for(int j = 0; j < size; j++)
		{
			src[i][j] = 255 * locality::utils::RRand(i, j);
			dst[i][j] = 0;
		}
}

template <typename T, typename AT>
double Check(AT dst, int size)
{
	T sum = 0;

	for(int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			sum += dst[i][j];
		}

		std::cout << (int)sum << std::endl;
		sum = 0;
	}

	return sum;
}

// https://en.wikipedia.org/wiki/Polytope_model

#define ERR(x,y) (dst[x][y] - src[x][y])

template <typename T, typename AT>
void NormalKernel(AT src, AT dst, int w, int h)
{
	LOC_PAPI_BEGIN_BLOCK

	for(int j = 0; j < h; ++j)
	{
		for(int i = 0; i < w; ++i)
		{
			int v = src[i][j];
		
			if(i > 0) v -= ERR(i - 1, j) /2;
			if(j > 0) v -= ERR(i, j - 1) / 4;

			if(j > 0 && i < w - 1)
				v -= ERR(i + 1, j - 1) / 4;

			dst[i][j] = (v < 128) ? 0 : 255;
			src[i][j] = (v < 0) ? 0 : (v < 255) ? v : 255;
		}
	}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void SkewedKernel(AT src, AT dst, int w, int h)
{
	LOC_PAPI_BEGIN_BLOCK

	for(int t = 0; t < (w + (2 * h)); ++t)
	{
		int pmin = std::max(t % 2, t - (2 * h) + 2);
		int pmax = std::min(t, w - 1);

		for(int p = pmin; p <= pmax; p += 2)
		{
			int i = p;
			int j = (t - p) / 2;
			int v = src[i][j];

			if(i > 0) v -= ERR(i - 1, j) / 2;
			if(j > 0) v -= ERR(i, j - 1) / 4;

			if(j > 0 && i < w - 1)
				v -= ERR(i + 1, j - 1) / 4;

			dst[i][j] = (v < 128) ? 0 : 255;
			src[i][j] = (v < 0) ? 0 : (v < 255) ? v : 255;
		}
	}

	LOC_PAPI_END_BLOCK
}

template <typename T, typename AT>
void CallKernel(int core_type, AT src, AT dst, int w, int h)
{
	switch(core_type)
	{
		case 0: NormalKernel<T, AT>(src, dst, w, h); break;

		case 1: SkewedKernel<T, AT>(src, dst, w, h); break;

		default: fprintf(stderr, "unexpected core type");
	}
}

#endif /*DITHERING*/
