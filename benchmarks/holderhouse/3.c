/* /tmp/3.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Table of constant values */

static real c_b2 = 1.f;

/* Subroutine */ int aa_(real *a, real *sx, real *sl, integer *n)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1;

    /* Builtin functions */
    double sqrt(doublereal), r_sign(real *, real *);

    /* Local variables */
    static real g;
    static integer i__, j, k;
    static real beta, alpha;

    /* Parameter adjustments */
    --sl;
    --sx;
    a_dim1 = *n;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    i__1 = *n - 1;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n;
	for (k = i__; k <= i__2; ++k) {
	    sx[k] = a[*n + i__ * a_dim1] * a[*n + k * a_dim1];
	}
	i__2 = i__;
	for (j = *n - 1; j >= i__2; --j) {
	    i__3 = *n;
	    for (k = i__; k <= i__3; ++k) {
		sx[k] += a[j + i__ * a_dim1] * a[j + k * a_dim1];
	    }
	}
	alpha = sqrt(sx[i__]);
	if (a[i__ + i__ * a_dim1] != 0.f) {
	    beta = 1.f / alpha;
	    i__2 = *n;
	    for (j = i__ + 1; j <= i__2; ++j) {
		a[j + i__ * a_dim1] *= beta;
	    }
	    sx[i__] = a[i__ + i__ * a_dim1] * beta + r_sign(&c_b2, &a[i__ + 
		    i__ * a_dim1]);
	    a[i__ + i__ * a_dim1] = alpha;
	    g = 1.f / (r__1 = sx[i__], dabs(r__1));
/* 1/gamma */
	    i__2 = *n;
	    for (k = i__ + 1; k <= i__2; ++k) {
		sx[k] = sx[k] * beta * g + r_sign(&a[i__ + k * a_dim1], &sx[
			i__]);
		a[i__ + k * a_dim1] += sx[k] * sx[i__];
		i__3 = *n;
		for (j = i__ + 1; j <= i__3; ++j) {
		    a[j + k * a_dim1] -= a[j + i__ * a_dim1] * sx[k];
		}
	    }
	} else {
	    if (alpha != 0.f) {
		beta = 1.f / alpha;
		i__2 = *n;
		for (j = i__ + 1; j <= i__2; ++j) {
		    a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = -1.f;
		a[i__ + i__ * a_dim1] = alpha;
		g = -1.f;
/* 1/gamma */
		i__2 = *n;
		for (k = i__ + 1; k <= i__2; ++k) {
		    sx[k] = sx[k] * beta * g + r_sign(&a[i__ + k * a_dim1], &
			    sx[i__]);
		    a[i__ + k * a_dim1] += sx[k] * sx[i__];
		    i__3 = *n;
		    for (j = i__ + 1; j <= i__3; ++j) {
			a[j + k * a_dim1] -= a[j + i__ * a_dim1] * sx[k];
		    }
		}
	    } else {
		sx[i__] = 1.f;
		g = 2.f;
		i__2 = *n;
		for (k = i__ + 1; k <= i__2; ++k) {
		    sx[k] = 2.f;
		    a[i__ + k * a_dim1] -= sx[k];
		}
	    }
	}
	if (i__ < *n - 1) {
	    i__2 = *n;
	    for (k = i__; k <= i__2; ++k) {
		sl[k] = a[i__ + *n * a_dim1] * a[k + *n * a_dim1];
	    }
	    i__2 = i__ + 1;
	    for (j = *n - 1; j >= i__2; --j) {
		i__3 = *n;
		for (k = i__; k <= i__3; ++k) {
		    sl[k] += a[i__ + j * a_dim1] * a[k + j * a_dim1];
		}
	    }
	    alpha = sqrt(sl[i__]);
	    if (a[i__ + (i__ + 1) * a_dim1] != 0.f) {
		beta = 1.f / alpha;
		i__2 = *n;
		for (j = i__ + 2; j <= i__2; ++j) {
		    a[i__ + j * a_dim1] *= beta;
		}
		sl[i__] = a[i__ + (i__ + 1) * a_dim1] * beta + r_sign(&c_b2, &
			a[i__ + (i__ + 1) * a_dim1]);
		a[i__ + (i__ + 1) * a_dim1] = alpha;
		g = 1.f / (r__1 = sl[i__], dabs(r__1));
/* 1/gamma */
		i__2 = *n;
		for (k = i__ + 1; k <= i__2; ++k) {
		    sl[k] = sl[k] * beta * g + r_sign(&a[k + (i__ + 1) * 
			    a_dim1], &sl[i__]);
		    a[k + (i__ + 1) * a_dim1] += sl[k] * sx[i__];
		    i__3 = *n;
		    for (j = i__ + 2; j <= i__3; ++j) {
			a[k + j * a_dim1] -= a[i__ + j * a_dim1] * sl[k];
		    }
		}
	    } else {
		if (alpha != 0.f) {
		    beta = 1.f / alpha;
		    i__2 = *n;
		    for (j = i__ + 2; j <= i__2; ++j) {
			a[i__ + j * a_dim1] *= beta;
		    }
		    sl[i__] = -1.f;
		    a[i__ + (i__ + 1) * a_dim1] = alpha;
		    g = -1.f;
/* 1/gamma */
		    i__2 = *n;
		    for (k = i__ + 1; k <= i__2; ++k) {
			sl[k] = sl[k] * beta * g + r_sign(&a[k + (i__ + 1) * 
				a_dim1], &sl[i__]);
			a[k + (i__ + 1) * a_dim1] += sl[k] * sl[i__];
			i__3 = *n;
			for (j = i__ + 2; j <= i__3; ++j) {
			    a[k + j * a_dim1] -= a[i__ + j * a_dim1] * sl[k];
			}
		    }
		} else {
		    sl[i__] = 1.f;
		    g = 2.f;
		    i__2 = *n;
		    for (k = i__ + 1; k <= i__2; ++k) {
			sl[k] = 2.f;
			a[k + (i__ + 1) * a_dim1] -= sl[k];
		    }
		}
	    }
	}
    }
    return 0;
} /* aa_ */

