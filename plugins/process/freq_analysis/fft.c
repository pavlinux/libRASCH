/*----------------------------------------------------------------------------
 * fft.c
 *
 * Implements FFT algorithm. Implementation needs input-sizes of N=2^n.
 * Now the fftw3 library is used, but if I have troubles with license issues,
 * this implementation will be used again.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/process/freq_analysis/fft.c,v 1.1 2003/09/10 13:14:57 rasch Exp $
 *--------------------------------------------------------------------------*/

#include <math.h>
#include "fft.h"


/* source: Signal Proc. Algorithms in Matlab, Disk., 1996 */
void
complex_exp(complex * r, complex * z)
{
	double expx;

	expx = exp(z->r);

	r->r = expx * cos(z->i);
	r->i = expx * sin(z->i);
}

/* Konjugiert komplexe Zahl */
/*		Eingabe: z komplexe Zahl */
/*		Ausgabe: r konjugiert komplexe Zahl */
/* Source: Signal Proc. Algorithms in Matlab, Disk., 1996 */
void
r_cnjg(complex * r, complex * z)
{
	r->r = z->r;
	r->i = -z->i;
}


/* SPFFTC     02/20/87 */
/* FAST FOURIER TRANSFORM OF N=2**K COMPLEX DATA POINTS USING TIME */
/* DECOMPOSITION WITH INPUT BIT REVERSAL.  N MUST BE A POWER OF 2. */
/* X MUST BE SPECIFIED COMPLEX X(0:N-1)OR LARGER. */
/* INPUT IS N COMPLEX SAMPLES, X(0),X(1),...,X(N-1). */
/* COMPUTATION IS IN PLACE, OUTPUT REPLACES INPUT. */
/* ISIGN = -1 FOR FORWARD TRANSFORM, +1 FOR INVERSE. */
/* X(0) BECOMES THE ZERO TRANSFORM COMPONENT, X(1) THE FIRST, */
/* AND SO FORTH.  X(N-1) BECOMES THE LAST COMPONENT. */

/* Source: Signal Proc. Algorithms in Matlab, Disk., 1996 */
void
spfftc(complex * x, long n, long isign)
{
	/* Local variables */
	long m, i, l, mr, tmp_int;
	complex t, tmp_complex, tmp;
	double pisign;

	pisign = (double) isign *PI;
	mr = 0;
	for (m = 1; m < n; ++m)
	{
		l = n;
		l /= 2;

		while (mr + l >= n)
			l /= 2;

		mr = mr % l + l;

		if (mr > m)
		{
			t.r = x[m].r;
			t.i = x[m].i;
			x[m].r = x[mr].r;
			x[m].i = x[mr].i;
			x[mr].r = t.r;
			x[mr].i = t.i;
		}
	}

	l = 1;
	while (l < n)
	{
		for (m = 0; m < l; ++m)
		{
			tmp_int = l * 2;

			for (i = m; tmp_int < 0 ? i >= (n - 1) : i < n; i += tmp_int)
			{
				tmp.r = 0.0;
				tmp.i = (double) m *pisign / (double) l;

				complex_exp(&tmp_complex, &tmp);

				t.r = x[i + l].r * tmp_complex.r - x[i + l].i * tmp_complex.i;
				t.i = x[i + l].r * tmp_complex.i + x[i + l].i * tmp_complex.r;

				x[i + l].r = x[i].r - t.r;
				x[i + l].i = x[i].i - t.i;

				x[i].r = x[i].r + t.r;
				x[i].i = x[i].i + t.i;
			}
		}
		l *= 2;
	}

	return;
} /* spfftc */


/* SPFFTR     11/12/85 */
/* FFT ROUTINE FOR REAL TIME SERIES (X) WITH N=2**K SAMPLES. */
/* COMPUTATION IS IN PLACE, OUTPUT REPLACES INPUT. */
/* INPUT:  REAL VECTOR X(0:N+1) WITH REAL DATA SEQUENCE IN FIRST N */
/*         ELEMENTS; ANYTHING IN LAST 2.  NOTE: X MAY BE DECLARED */
/*         REAL IN MAIN PROGRAM PROVIDED THIS ROUTINE IS COMPILED  */
/*         SEPARATELY ... COMPLEX OUTPUT REPLACES REAL INPUT HERE. */
/* OUTPUT: COMPLEX VECTOR XX(O:N/2), SUCH THAT X(0)=REAL(XX(0)),X(1)= */
/*         IMAG(XX(0)), X(2)=REAL(XX(1)), ..., X(N+1)=IMAG(XX(N/2). */
/* IMPORTANT:  N MUST BE AT LEAST 4 AND MUST BE A POWER OF 2. */

/* Source: Signal Proc. Algorithms in Matlab, Disk., 1996 */
void
spfftr(complex * x, long n, long inverse)
{
	int m;
	long tmp_int;
	complex u, tmp, tmp_complex;
	double tpn, tmp_float;

	tpn = (float) (2.0 * PI / (double) n);

	tmp_int = n / 2;
	spfftc(x, tmp_int, inverse);

	x[n / 2].r = x[0].r;
	x[n / 2].i = x[0].i;

	for (m = 0; m <= (n / 4); ++m)
	{
		u.r = (float) sin((double) m * tpn);
		u.i = (float) cos((double) m * tpn);

		r_cnjg(&tmp_complex, &x[n / 2 - m]);

		tmp.r = (((1.0 + u.r) * x[m].r - u.i * x[m].i)
			 + (1.0 - u.r) * tmp_complex.r - -u.i * tmp_complex.i) / 2.0;

		tmp.i = (((1.0 + u.r) * x[m].i + u.i * x[m].r)
			 + (1.0 - u.r) * tmp_complex.i + -u.i * tmp_complex.r) / 2.0;

		tmp_float = ((1.0 - u.r) * x[m].r - -u.i * x[m].i
			     + (1.0 + u.r) * tmp_complex.r - u.i * tmp_complex.i) / 2.0;
		x[m].i = ((1.0 - u.r) * x[m].i + -u.i * x[m].r +
			  (1.0 + u.r) * tmp_complex.i + u.i * tmp_complex.r) / 2.0;
		x[m].r = tmp_float;

		r_cnjg(&x[n / 2 - m], &tmp);
	}

	return;
} /* spfftr */


