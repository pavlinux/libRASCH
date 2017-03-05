/*----------------------------------------------------------------------------
 * frequency.c
 *
 * Performs frequency calculations.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "frequency.h"
#include "statistics.h"
#include <ra_defines.h>


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


int
remove_trend(double *v, long n)
{
	double *x = NULL;
	double *y = NULL;
	long l;
	double offset, slope, single_slope;

	/* calc linear regression */
	x = malloc(sizeof(double) * n);;
	y = malloc(sizeof(double) * n);
	for (l = 0; l < n; l++)
	{
		x[l] = (double) l;
		y[l] = v[l];
	}

	calc_regression(x, y, n, &offset, &slope);
	free(x);
	free(y);

	/* remove linear regression */
	single_slope = slope / (double) n;
	for (l = 0; l < n; l++)
		v[l] += ((double) l * single_slope);

	return 0;
} /* remove_trend() */


int
remove_mean(double *v, long n)
{
	double m;
	long l;

	m = mean(v, n);
	for (l = 0; l < n; l++)
		v[l] -= m;

	return 0;
} /* remove_mean() */


int
make_equidistant(double *v, double *pos, long n, double distance, double **equi, long *n_equi)
{
	long l, idx;
	double before, after;
	double pos_before, pos_after, curr_pos;
	double scale;
	double *t;

	*n_equi = (long) ((pos[n - 1] - pos[0]) / distance);
	*equi = malloc(sizeof(double) * (*n_equi));

	before = v[0];
	after = v[1];
	pos_before = pos[0];
	pos_after = pos[1];
	curr_pos = pos[0];
	idx = 1;
	scale = (after - before) / (pos_after - pos_before);
	for (l = 0; l < *n_equi; l++)
	{
		while (curr_pos > pos_after)
		{
			if (idx == (n - 1))
				break;

			before = v[idx];
			pos_before = pos[idx];

			after = v[idx + 1];
			pos_after = pos[idx + 1];

			idx++;

			scale = (after - before) / (pos_after - pos_before);
		}

		(*equi)[l] = before + ((curr_pos - pos_before) * scale);

		curr_pos += distance;
	}
	*n_equi = l;

	/* boxcar filter data */
	t = malloc(sizeof(double) * (*n_equi));
	t[0] = (*equi)[0];
	for (l = 1; l < *n_equi; l++)
		t[l] = ((*equi)[l - 1] + (*equi)[l]) / 2;
	free(*equi);
	*equi = t;

	return 0;
} /* make_equidistant() */


int
make_instantaneous_period(double *v, double *pos, long n, double distance, double **equi, long *n_equi)
{
	long l, idx;
	double curr_pos;
	double *t;

	*n_equi = (long) ((pos[n - 1] - pos[0]) / distance);
	*equi = malloc(sizeof(double) * (*n_equi));
	(*equi)[0] = v[0];
	idx = 1;
	curr_pos = pos[0] + distance;
	for (l = 1; l < *n_equi; l++)
	{
		double p1, p2;
		double d1, d2;
		double hr;

		p1 = curr_pos - distance;
		p2 = curr_pos + distance;

		d1 = pos[idx] - p1;
		if (d1 >= 2 * distance)
			hr = (2.0 * distance) / ((2 * distance) / v[idx]);
		else
		{
			double local_value;

			if ((idx + 1) >= n)
				break;

			d2 = p2 - pos[idx];
			local_value = d1 / v[idx] + d2 / v[idx + 1];
			idx++;
			hr = (2.0 * distance) / local_value;
		}

		(*equi)[l] = hr;

		curr_pos += distance;
	}

	/* boxcar filter data */
	t = malloc(sizeof(double) * (*n_equi));
	t[0] = (*equi)[0];
	for (l = 1; l < *n_equi; l++)
		t[l] = ((*equi)[l - 1] + (*equi)[l]) / 2;
	free(*equi);
	*equi = t;

	return 0;
} /* make_instantaneous_hr() */


long
calc_idx_from_freq(double samplefreq, long n_sample, double freq)
{
	double freq_step;
	long idx;

	freq_step = samplefreq / (double) n_sample;
	idx = (long) ((freq / freq_step) + 0.5);

	return idx;
} /* calc_idx_from_freq() */


int
calc_periodogram(double *v, long n, double samplefreq, double **spec, long *n_spec, char *window)
{
	long pow2, n_fft, l;
	double r;
	complex *x = NULL;
	double w_power;

	r = log(n) / log(2);
	pow2 = (long)r;
	if ((double)pow2 != r)
		pow2++;
	n_fft = (long)pow(2.0, (double)pow2);

	remove_mean(v, n);
	window_data(v, n, window, &w_power);

	x = calloc(n_fft, sizeof(complex));
	for (l = 0; l < n; l++)
		x[l].r = v[l];
	
	spfftc(x, n_fft, -1);	/* forward FFT */

	if (((n_fft/2) % 2) == 1)
		*n_spec = (n_fft + 1) / 2;
	else
		*n_spec = (n_fft/2);

	*spec = malloc(sizeof(double) * (*n_spec));
	for (l = 0; l < (*n_spec); l++)
	{
		/* correct spectrum regarding window-power and number of samples */
		(*spec)[l] = (pow(x[l].r, 2.0) + pow(x[l].i, 2.0)) / (w_power * (double)n);
		if ((l != 0) && (l != (*n_spec - 1)))
			(*spec)[l] *= 2.0;
		(*spec)[l] /= samplefreq;
	}

	if (x)
		free(x);

	return 0;
} /* calc_periodogram() */


int
calc_welch_spec(double *v, long n, double samplefreq, double **spec, long *n_spec, long seg_len,
		long overlap, char *window)
{
	/* calc averaged spectrum of len sequences with an overlap of overlap */
	long n_seg, l, m;
	double *single_spec;
	double *t;

	/* get number of segments */
	n_seg = (n - overlap) / (seg_len - overlap);

	/* calc spectra */
	t = malloc(sizeof(double) * seg_len);
	*spec = NULL;
	for (l = 0; l < n_seg; l++)
	{
		for (m = 0; m < seg_len; m++)
			t[m] = v[l * (seg_len - overlap) + m];
		calc_periodogram(t, seg_len, samplefreq, &single_spec, n_spec, window);
		if (*spec == NULL)
		{
			*spec = malloc(sizeof(double) * (*n_spec));
			memset(*spec, 0, sizeof(double) * (*n_spec));
		}
		for (m = 0; m < *n_spec; m++)
			(*spec)[m] += single_spec[m];
		free(single_spec);
	}
	for (m = 0; m < *n_spec; m++)
		(*spec)[m] /= (double) n_seg;

	free(t);

	return 0;
} /* calc_welch_spec() */


int
window_data(double *v, long n, char *window, double *w_power)
{
	double *w;
	long l;

	if ((window == NULL) || (window[0] == '\0'))
		return -1;

	w = malloc(sizeof(double) * n);
	memset(w, 0, sizeof(double) * n);
	*w_power = 1.0;

	if (RA_STRICMP(window, "hanning") == 0)
		window_hanning(w, n, w_power);
	else if (RA_STRICMP(window, "hamming") == 0)
		window_hamming(w, n, w_power);
	else if (RA_STRICMP(window, "bartlett") == 0)
		window_bartlett(w, n, w_power);
	else if (RA_STRICMP(window, "blackman") == 0)
		window_blackman(w, n, w_power);
	else
		return -1;	/* no window found */

	for (l = 0; l < n; l++)
		v[l] *= w[l];

	free(w);

	return 0;
} /* window_data() */


void
window_hanning(double *v, long n, double *power)
{
	long l;
	long n2 = n / 2;

	v[n - 1] = 0.0;

	*power = 0.0;
	for (l = -n2; l < n2; l++)
	{
		v[l + n2] = 0.5 + 0.5 * cos(PI * (double) l / (double) n2);
		*power += (v[l + n2] * v[l + n2]);
	}
	*power /= (double)n;
} /* window_hanning() */


void
window_hamming(double *v, long n, double *power)
{
	long l;
	long n2 = n / 2;

	v[n - 1] = 0.0;

	*power = 0.0;
	for (l = -n2; l < n2; l++)
	{
		v[l + n2] = 0.54 + 0.46 * cos(PI * (double) l / (double) n2);
		*power += (v[l + n2] * v[l + n2]);
	}
	*power /= (double)n;
} /* window_hamming() */


void
window_bartlett(double *v, long n, double *power)
{
	long l;
	long n2 = n / 2;

	v[n - 1] = 0.0;

	*power = 0.0;
	for (l = -n2; l < n2; l++)
	{
		v[l + n2] = 1.0 - (double) abs(l) / (double) n2;
		*power += (v[l + n2] * v[l + n2]);
	}
	*power /= (double)n;
} /* window_bartlett() */


void
window_blackman(double *v, long n, double *power)
{
	long l;
	long n2 = n / 2;

	*power = 0.0;
	for (l = -n2; l < n2; l++)
	{
		v[l + n2] = 0.42 + 0.5 * cos(PI * (double) l / (double) n2)
			+ 0.08 * cos(2.0 * PI * (double) l / (double) n2);
		*power += (v[l + n2] * v[l + n2]);
	}
	*power /= (double)n;
} /* window_blackman() */

int
calc_log_freq_smoothed_spec(const double *spec, long n, double samplerate, double **smoothed, long *n_sm)
{
	double freq_step;
	int n_decades;
	int begin, end;
	int *cnt = NULL;
	long l;

	freq_step = samplerate / ((double)n * 2.0);

	begin = (int)(log10(freq_step)-1.0);
	end = (int)(log10(samplerate)+1.0);

	n_decades = end - begin;
	*n_sm = n_decades * 60;

	(*smoothed) = malloc(sizeof(double) * (*n_sm));
	memset(*smoothed, 0, sizeof(double)*(*n_sm));
	cnt = malloc(sizeof(int) * (*n_sm));
	memset(cnt, 0, sizeof(int)*(*n_sm));

	/* do it brute force (TODO: find more elegant method) */
	for (l = 1; l < n; l++)
	{
		double curr = freq_step * (double)l;
		double diff = log10(curr) - (double)begin;
		long idx = (long)(diff * 60);

		(*smoothed)[idx] += spec[l];
		cnt[idx] += 1;
	}

	for (l = 0; l < (*n_sm); l++)
	{
		if (cnt[l] > 0)
			(*smoothed)[l] /= (double)cnt[l];
		else
			(*smoothed)[l] = -1.0;
	}

	if (cnt)
		free(cnt);

	return 0;
}  /* calc_log_freq_smoothed_spec() */
