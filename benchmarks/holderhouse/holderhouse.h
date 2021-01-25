#ifndef HOLDERHOUSE_H
#define HOLDERHOUSE_H

#include <cmath>

const int BENCH_COUNT = 3;

const char* type_names[BENCH_COUNT] = {
	"qr", // 0
	"qtq", //1
	"qdu", //2
};

template <typename T, typename AT, typename AT2>
void Init(AT2 a, AT b, AT aux, AT u, size_t n)
{
	for(size_t i = 0; i < n; i++)
		for(size_t j = 0; j < n; j++)
			a[i+j*n] = locality::utils::RRand(i, j);

	for(size_t i = 0; i < n; i++)
	{
		u[i] = locality::utils::RRand(i);
		b[i] = locality::utils::RRand(i);
	}

	for(size_t i = 0; i < n; i++)
		aux[i] = locality::utils::RRand(i);

}

template <typename T, typename AT2>
T Check(AT2 a, size_t n)
{
	T sum = 0;

	for(size_t i = 0; i < n; i++)
		for(size_t j = 0; j < n; j++)
			sum += a[i+j*n];

	return sum;
}

#define sgn(a) ((a) == 0 ? 0 : ((a) > 0  ? 1 : -1 ))

#define r_sign(a,b) (abs(a) * sgn(b))

template <typename T, typename AT, typename AT2>
void KernelQR(AT2 a, AT sx, AT , AT , size_t n)
{
	int a_dim1, i__1, i__2, i__3;

	/* Local variables */
	static T g;
	static int i__, j, k;
	static T beta, alpha;

	/* Parameter adjustments */
	a_dim1 = n;

	/* Function Body */
	i__1 = n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (k = i__; k <= i__2; ++k) {
		sx[k] = a[n + i__ * a_dim1] * a[n + k * a_dim1];
	}
	i__2 = i__;
	for (j = n - 1; j >= i__2; --j) {
		i__3 = n;
		for (k = i__; k <= i__3; ++k) {
		sx[k] = sx[k] + a[j + i__ * a_dim1] * a[j + k * a_dim1];
		}
	}
	alpha = sqrt(sx[i__]);
	if (a[i__ + i__ * a_dim1] != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 1; j <= i__2; ++j) {
		a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = a[i__ + i__ * a_dim1] * beta + r_sign(1.0, a[i__ + 
			i__ * a_dim1]);
		a[i__ + i__ * a_dim1] = alpha;
		g = 1.f / (abs(sx[i__]));
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
		sx[k] = sx[k] * beta * g + r_sign(a[i__ + k * a_dim1], sx[
			i__]);
		a[i__ + k * a_dim1] = a[i__ + k * a_dim1] + sx[k] * sx[i__];
		i__3 = n;
		for (j = i__ + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] = a[j + k * a_dim1] - a[j + i__ * a_dim1] * sx[k];
		}
		}
	} else {
		if (alpha != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 1; j <= i__2; ++j) {
			a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = -1.f;
		a[i__ + i__ * a_dim1] = alpha;
		g = -1.f;
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
			sx[k] = sx[k] * beta * g + r_sign(a[i__ + k * a_dim1], 
				sx[i__]);
			a[i__ + k * a_dim1] = a[i__ + k * a_dim1] + sx[k] * sx[i__];
			i__3 = n;
			for (j = i__ + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] = a[j + k * a_dim1] - a[j + i__ * a_dim1] * sx[k];
			}
		}
		} else {
		sx[i__] = 1.f;
		g = 2.f;
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
			sx[k] = 2.f;
			a[i__ + k * a_dim1] = a[i__ + k * a_dim1] - sx[k];
		}
		}
	}
	}
}

template <typename T, typename AT, typename AT2>
void KernelQTQ(AT2 a, AT sx, AT , AT , size_t n)
{
   /* System generated locals */
	int a_dim1, i__1, i__2, i__3;


	/* Local variables */
	static T g;
	static int i__, j, k;
	static T beta, alpha;

	/* Parameter adjustments */
	a_dim1 = n;

	/* Function Body */
	i__1 = n - 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (k = i__; k <= i__2; ++k) {
		sx[k] = a[n + i__ * a_dim1] * a[n + k * a_dim1];
	}
	i__2 = i__ + 1;
	for (j = n - 1; j >= i__2; --j) {
		sx[i__] = sx[i__] + a[j + i__ * a_dim1] * a[j + i__ * a_dim1];
	}
	i__2 = n;
	for (k = i__ + 1; k <= i__2; ++k) {
		i__3 = k;
		for (j = n - 1; j >= i__3; --j) {
		sx[k] = sx[k] + a[j + i__ * a_dim1] * a[j + k * a_dim1];
		}
		i__3 = i__ + 1;
		for (j = k - 1; j >= i__3; --j) {
		sx[k] = sx[k] + a[j + i__ * a_dim1] * a[k + j * a_dim1];
		}
	}
	alpha = sqrt(sx[i__]);
	if (a[i__ + 1 + i__ * a_dim1] != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 2; j <= i__2; ++j) {
		a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = a[i__ + 1 + i__ * a_dim1] * beta + r_sign(1.0, a[i__ 
			+ 1 + i__ * a_dim1]);
		a[i__ + 1 + i__ * a_dim1] = alpha;
		g = 1.f / (abs(sx[i__]));
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 2; k <= i__2; ++k) {
		sx[k] = sx[k] * beta * g + r_sign(a[k + (i__ + 1) * a_dim1], 
			sx[i__]);
		a[k + k * a_dim1] = a[k + k * a_dim1] - a[k + i__ * a_dim1] * 2 * sx[k];
		}
		i__2 = n;
		for (k = i__ + 2; k <= i__2; ++k) {
		i__3 = n;
		for (j = k + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] = a[j + k * a_dim1] - a[j + i__ * 
				a_dim1] * sx[k] - a[k + i__ * a_dim1] * sx[i__];
		}
		}
	} else {
		if (alpha != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 2; j <= i__2; ++j) {
			a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = -1.f;
		a[i__ + 1 + i__ * a_dim1] = alpha;
		g = -1.f;
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 2; k <= i__2; ++k) {
			sx[k] = sx[k] * beta * g + r_sign(a[k + (i__ + 1) * 
				a_dim1], sx[i__]);
			a[k + k * a_dim1] = a[k + k * a_dim1] - a[k + i__ * a_dim1] * 2 * sx[k];
		}
		i__2 = n;
		for (k = i__ + 2; k <= i__2; ++k) {
			i__3 = n;
			for (j = k + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] = a[j + k * a_dim1] - a[j + i__ * 
				a_dim1] * sx[k] - a[k + i__ * a_dim1] * sx[
				i__];
			}
		}
		} else {
		sx[i__] = 1.f;
		}
	}
	}
}

template <typename T, typename AT, typename AT2>
void KernelQDU(AT2 a, AT sx, AT sl, AT , size_t n)
{
  /* System generated locals */
	size_t a_dim1,  i__1, i__2, i__3;

	/* Local variables */
	static T g;
	static size_t i__, j, k;
	static float beta, alpha;

	/* Parameter adjustments */
	a_dim1 = n;

	/* Function Body */
	i__1 = n - 1;
	for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = n;
	for (k = i__; k <= i__2; ++k) {
		sx[k] = a[n + i__ * a_dim1] * a[n + k * a_dim1];
	}
	i__2 = i__;
	for (j = n - 1; j >= i__2; --j) {
		i__3 = n;
		for (k = i__; k <= i__3; ++k) {
		sx[k] += a[j + i__ * a_dim1] * a[j + k * a_dim1];
		}
	}
	alpha = sqrt(sx[i__]);
	if (a[i__ + i__ * a_dim1] != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 1; j <= i__2; ++j) {
		a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = a[i__ + i__ * a_dim1] * beta + r_sign(1.0, a[i__ + 
			i__ * a_dim1]);
		a[i__ + i__ * a_dim1] = alpha;
		g = 1.f / abs(sx[i__]);
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
		sx[k] = sx[k] * beta * g + r_sign(a[i__ + k * a_dim1], sx[
			i__]);
		a[i__ + k * a_dim1] += sx[k] * sx[i__];
		i__3 = n;
		for (j = i__ + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] -= a[j + i__ * a_dim1] * sx[k];
		}
		}
	} else {
		if (alpha != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 1; j <= i__2; ++j) {
			a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = -1.f;
		a[i__ + i__ * a_dim1] = alpha;
		g = -1.f;
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
			sx[k] = sx[k] * beta * g + r_sign(a[i__ + k * a_dim1], 
				sx[i__]);
			a[i__ + k * a_dim1] += sx[k] * sx[i__];
			i__3 = n;
			for (j = i__ + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] -= a[j + i__ * a_dim1] * sx[k];
			}
		}
		} else {
		sx[i__] = 1.f;
		g = 2.f;
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
			sx[k] = 2.f;
			a[i__ + k * a_dim1] -= sx[k];
		}
		}
	}
	if (i__ < n - 1) {
		i__2 = n;
		for (k = i__; k <= i__2; ++k) {
		sl[k] = a[i__ + n * a_dim1] * a[k + n * a_dim1];
		}
		i__2 = i__ + 1;
		for (j = n - 1; j >= i__2; --j) {
		i__3 = n;
		for (k = i__; k <= i__3; ++k) {
			sl[k] += a[i__ + j * a_dim1] * a[k + j * a_dim1];
		}
		}
		alpha = sqrt(sl[i__]);
		if (a[i__ + (i__ + 1) * a_dim1] != 0.f) {
		beta = 1.f / alpha;
		i__2 = n;
		for (j = i__ + 2; j <= i__2; ++j) {
			a[i__ + j * a_dim1] *= beta;
		}
		sl[i__] = a[i__ + (i__ + 1) * a_dim1] * beta + r_sign(1.0, 
			a[i__ + (i__ + 1) * a_dim1]);
		a[i__ + (i__ + 1) * a_dim1] = alpha;
		g = 1.f / abs(sl[i__]);
/* 1/gamma */
		i__2 = n;
		for (k = i__ + 1; k <= i__2; ++k) {
			sl[k] = sl[k] * beta * g + r_sign(a[k + (i__ + 1) * 
				a_dim1], sl[i__]);
			a[k + (i__ + 1) * a_dim1] += sl[k] * sx[i__];
			i__3 = n;
			for (j = i__ + 2; j <= i__3; ++j) {
			a[k + j * a_dim1] -= a[i__ + j * a_dim1] * sl[k];
			}
		}
		} else {
		if (alpha != 0.f) {
			beta = 1.f / alpha;
			i__2 = n;
			for (j = i__ + 2; j <= i__2; ++j) {
			a[i__ + j * a_dim1] *= beta;
			}
			sl[i__] = -1.f;
			a[i__ + (i__ + 1) * a_dim1] = alpha;
			g = -1.f;
/* 1/gamma */
			i__2 = n;
			for (k = i__ + 1; k <= i__2; ++k) {
			sl[k] = sl[k] * beta * g + r_sign(a[k + (i__ + 1) * 
				a_dim1], sl[i__]);
			a[k + (i__ + 1) * a_dim1] += sl[k] * sl[i__];
			i__3 = n;
			for (j = i__ + 2; j <= i__3; ++j) {
				a[k + j * a_dim1] -= a[i__ + j * a_dim1] * sl[k];
			}
			}
		} else {
			sl[i__] = 1.f;
			g = 2.f;
			i__2 = n;
			for (k = i__ + 1; k <= i__2; ++k) {
			sl[k] = 2.f;
			a[k + (i__ + 1) * a_dim1] -= sl[k];
			}
		}
		}
	}
	}
}

template <typename T, typename AT, typename AT2>
void CallKernel(size_t core_type, AT2 a, AT b, AT aux, AT u, size_t n)
{
	LOC_PAPI_BEGIN_BLOCK

	switch(core_type)
	{
		case 0: KernelQR<T, AT, AT2>(a, b, aux, u, n); break;

		case 1: KernelQTQ<T, AT, AT2>(a, b, aux, u, n); break;

		case 2: KernelQDU<T, AT, AT2>(a, b, aux, u, n); break;

		default: fprintf(stderr, "unexpected core type");
	}

	LOC_PAPI_END_BLOCK
}

#endif /*HOLDERHOUSE_H*/
