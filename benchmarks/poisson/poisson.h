#include <math.h>

#include "fft_complex.h"

template<typename T, typename AT, typename AT3>
void Init(AT3 input, AT tmp_x, AT tmp_y, AT tmp_z
	, AT pov_x, AT pov_y, AT pov_z
	, size_t size_x, size_t size_y, size_t size_z)
{
	for(size_t z = 0; z < size_z; z++)
		for(size_t y = 0; y < size_y; y++)
			for(size_t x = 0; x < size_x; x++)
			{
				input[x][y][z * 2 + 0] = locality::utils::RRand(z, y * x);
				input[x][y][z * 2 + 1] = locality::utils::RRand(z * y, x);
			}

	for(size_t z = 0; z < size_z; z++)
	{
		tmp_z[z * 2 + 0] = locality::utils::RRand(z);
		tmp_z[z * 2 + 1] = locality::utils::RRand(z);

		pov_z[z * 2 + 0] = locality::utils::RRand(z);
		pov_z[z * 2 + 1] = locality::utils::RRand(z);
	}

	for(size_t y = 0; y < size_y; y++)
	{
		tmp_y[y * 2 + 0] = locality::utils::RRand(y);
		tmp_y[y * 2 + 1] = locality::utils::RRand(y);

		pov_y[y * 2 + 0] = locality::utils::RRand(y);
		pov_y[y * 2 + 1] = locality::utils::RRand(y);
	}

	for(size_t x = 0; x < size_x; x++)
	{
		tmp_x[x * 2 + 0] = locality::utils::RRand(x);
		tmp_x[x * 2 + 1] = locality::utils::RRand(x);

		pov_x[x * 2 + 0] = locality::utils::RRand(x);
		pov_x[x * 2 + 1] = locality::utils::RRand(x);
	}
}

template<typename T, typename AT3>
T Check(AT3 input, size_t size_x, size_t size_y, size_t size_z)
{
	T sum = 0;

	for(size_t z = 0; z < size_z; z++)
		for(size_t y = 0; y < size_y; y++)
			for(size_t x = 0; x < size_x; x++)
				sum += input[x][y][z * 2 + 0] + input[x][y][z * 2 + 1];

	return sum;
}

static const double PI = std::atan(1.0) * 4.0;

/* FFT recursion */
template<typename T, typename AT, typename AT3>
void Kernel(AT3 input, AT tmp_x, AT tmp_y, AT tmp_z
	, AT pov_x, AT pov_y, AT pov_z
	, size_t size_x, size_t size_y, size_t size_z)
{
	LOC_PAPI_BEGIN_BLOCK

// x - transform
	for(size_t z = 0; z < size_z; z++)
		for(size_t y = 0; y < size_y; y++)
		{
			for(size_t x = 0; x < size_x; x++)
			{
				tmp_x[x * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_x[x * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::FFT<T, AT>(tmp_x, pov_x, size_x);

			for(size_t x = 0; x < size_x; x++)
			{
				input[x][y][z * 2 + 0] = tmp_x[x * 2 + 0];
				input[x][y][z * 2 + 1] = tmp_x[x * 2 + 1];
			}
		}

// y - transform
	for(size_t z = 0; z < size_z; z++)
		for(size_t x = 0; x < size_x; x++)
		{
			for(size_t y = 0; y < size_y; y++)
			{
				tmp_y[y * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_y[y * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::FFT<T, AT>(tmp_y, pov_y, size_y);

			for(size_t y = 0; y < size_y; y++)
			{
				input[x][y][z * 2 + 0] = tmp_y[y * 2 + 0];
				input[x][y][z * 2 + 1] = tmp_y[y * 2 + 1];
			}
		}

// z - transform and dividing by eigenvalue
	for(size_t y = 0; y < size_y; y++)
		for(size_t x = 0; x < size_x; x++)
		{
			for(size_t z = 0; z < size_z; z++)
			{
				tmp_z[z * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_z[z * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::FFT<T, AT>(tmp_z, pov_z, size_z);

// Dividing by eigenvalue of Laplace operator

			T sx = std::sin(PI * x / size_x);
			T sy = std::sin(PI * y / size_y);

			for(size_t z = 0; z < size_z; z++)
			{
				T sz = std::sin(PI * z / size_z);

				T div = 4.0 / (sx*sx + sy*sy + sz*sz);

				input[x][y][z * 2 + 0] = tmp_z[z * 2 + 0] / div;
				input[x][y][z * 2 + 1] = tmp_z[z * 2 + 1] / div;
			}
		}

// inverse part

// inverse x - transform
	for(size_t z = 0; z < size_z; z++)
		for(size_t y = 0; y < size_y; y++)
		{
			for(size_t x = 0; x < size_x; x++)
			{
				tmp_x[x * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_x[x * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::IFFT<T, AT>(tmp_x, pov_x, size_x);

			for(size_t x = 0; x < size_x; x++)
			{
				input[x][y][z * 2 + 0] = tmp_x[x * 2 + 0];
				input[x][y][z * 2 + 1] = tmp_x[x * 2 + 1];
			}
		}

// inverse y - transform
	for(size_t z = 0; z < size_z; z++)
		for(size_t x = 0; x < size_x; x++)
		{
			for(size_t y = 0; y < size_y; y++)
			{
				tmp_y[y * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_y[y * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::IFFT<T, AT>(tmp_y, pov_y, size_y);

			for(size_t y = 0; y < size_y; y++)
			{
				input[x][y][z * 2 + 0] = tmp_y[y * 2 + 0];
				input[x][y][z * 2 + 1] = tmp_y[y * 2 + 1];
			}
		}

// inverse z - transform
	for(size_t y = 0; y < size_y; y++)
		for(size_t x = 0; x < size_x; x++)
		{
			for(size_t z = 0; z < size_z; z++)
			{
				tmp_z[z * 2 + 0] = input[x][y][z * 2 + 0];
				tmp_z[z * 2 + 1] = input[x][y][z * 2 + 1];
			}

			fft_complex::IFFT<T, AT>(tmp_z, pov_z, size_z);

			for(size_t z = 0; z < size_z; z++)
			{
				input[x][y][z * 2 + 0] = tmp_z[z * 2 + 0];
				input[x][y][z * 2 + 1] = tmp_z[z * 2 + 1];
			}
		}

	LOC_PAPI_END_BLOCK
}
