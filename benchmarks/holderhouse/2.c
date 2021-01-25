/* /tmp/2.f -- translated by f2c (version 20100827).
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

/* Subroutine */ int aa_(real *a, real *sx, integer *n)
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
    --sx;
    a_dim1 = *n;
    a_offset = 1 + a_dim1;
    a -= a_offset;

    /* Function Body */
    i__1 = *n - 2;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *n;
	for (k = i__; k <= i__2; ++k) {
	    sx[k] = a[*n + i__ * a_dim1] * a[*n + k * a_dim1];
	}
	i__2 = i__ + 1;
	for (j = *n - 1; j >= i__2; --j) {
	    sx[i__] += a[j + i__ * a_dim1] * a[j + i__ * a_dim1];
	}
	i__2 = *n;
	for (k = i__ + 1; k <= i__2; ++k) {
	    i__3 = k;
	    for (j = *n - 1; j >= i__3; --j) {
		sx[k] += a[j + i__ * a_dim1] * a[j + k * a_dim1];
	    }
	    i__3 = i__ + 1;
	    for (j = k - 1; j >= i__3; --j) {
		sx[k] += a[j + i__ * a_dim1] * a[k + j * a_dim1];
	    }
	}
	alpha = sqrt(sx[i__]);
	if (a[i__ + 1 + i__ * a_dim1] != 0.f) {
	    beta = 1.f / alpha;
	    i__2 = *n;
	    for (j = i__ + 2; j <= i__2; ++j) {
		a[j + i__ * a_dim1] *= beta;
	    }
	    sx[i__] = a[i__ + 1 + i__ * a_dim1] * beta + r_sign(&c_b2, &a[i__ 
		    + 1 + i__ * a_dim1]);
	    a[i__ + 1 + i__ * a_dim1] = alpha;
	    g = 1.f / (r__1 = sx[i__], dabs(r__1));
/* 1/gamma */
	    i__2 = *n;
	    for (k = i__ + 2; k <= i__2; ++k) {
		sx[k] = sx[k] * beta * g + r_sign(&a[k + (i__ + 1) * a_dim1], 
			&sx[i__]);
		a[k + k * a_dim1] -= a[k + i__ * a_dim1] * 2 * sx[k];
	    }
	    i__2 = *n;
	    for (k = i__ + 2; k <= i__2; ++k) {
		i__3 = *n;
		for (j = k + 1; j <= i__3; ++j) {
		    a[j + k * a_dim1] = a[j + k * a_dim1] - a[j + i__ * 
			    a_dim1] * sx[k] - a[k + i__ * a_dim1] * sx[i__];
		}
	    }
	} else {
	    if (alpha != 0.f) {
		beta = 1.f / alpha;
		i__2 = *n;
		for (j = i__ + 2; j <= i__2; ++j) {
		    a[j + i__ * a_dim1] *= beta;
		}
		sx[i__] = -1.f;
		a[i__ + 1 + i__ * a_dim1] = alpha;
		g = -1.f;
/* 1/gamma */
		i__2 = *n;
		for (k = i__ + 2; k <= i__2; ++k) {
		    sx[k] = sx[k] * beta * g + r_sign(&a[k + (i__ + 1) * 
			    a_dim1], &sx[i__]);
		    a[k + k * a_dim1] -= a[k + i__ * a_dim1] * 2 * sx[k];
		}
		i__2 = *n;
		for (k = i__ + 2; k <= i__2; ++k) {
		    i__3 = *n;
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
    return 0;
} /* aa_ */

