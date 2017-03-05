/*----------------------------------------------------------------------------
 * frequency.c
 *
 * Performs frequency calculations. For FFT, the fftw3 library is used.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "frequency_fftw3.h"
#include "statistics.h"
#include <ra_defines.h>

#ifdef PL_USE_FFTW3
#include <fftw3.h>
#else
#include "sing.h"
#endif

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
calc_periodogram(double *v, long n, double samplefreq, double **spec, long *n_spec, int rm_mean, char *window)
{
	long n_fft, l;
	double w_power = 1.0;
	complex *out = NULL;
	double *v_use = NULL;

	if (rm_mean)
		remove_mean(v, n);
	if ((window != NULL) && (window[0] != '\0'))
		window_data(v, n, window, &w_power);

	if (*n_spec > n)
		n_fft = *n_spec;
	else
		n_fft = n;
	v_use = (double *)calloc(n_fft, sizeof(double));
	memcpy(v_use, v, sizeof(double) * n);

	out = malloc(sizeof(complex) * n_fft);
	memset(out, 0, sizeof(complex) * n_fft);
#ifdef PL_USE_FFTW3
	fftw(v_use, n_fft, out);
#else
	calc_fft(v_use, n_fft, out);
#endif

	if (((n_fft/2) % 2) == 1)
		*n_spec = (n_fft + 1) / 2;
	else
		*n_spec = (n_fft/2);

	*spec = malloc(sizeof(double) * (*n_spec));
	for (l = 0; l < (*n_spec); l++)
	{
		/* correct spectrum regarding window-power and number of samples */
		(*spec)[l] = (pow(out[l].r, 2.0) + pow(out[l].i, 2.0)) / (w_power * (double)n);
		if ((l != 0) && (l != (*n_spec - 1)))
			(*spec)[l] *= 2.0;
		(*spec)[l] /= samplefreq;
	}

	if (out)
		free(out);
	if (v_use)
		free(v_use);

	return 0;
} /* calc_periodogram() */


int
calc_welch_spec(double *v, long n, double samplefreq, double **spec, long *n_spec, long seg_len,
		long overlap, int rm_mean, char *window)
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
		calc_periodogram(t, seg_len, samplefreq, &single_spec, n_spec, rm_mean, window);
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


#ifdef PL_USE_FFTW3

void
fftw(double *data, long n, complex *out)
{
	long l;
	fftw_complex *out_fftw;
	fftw_plan p;

	out_fftw = fftw_malloc(sizeof(fftw_complex) * n);

	p = fftw_plan_dft_r2c_1d(n, data, out_fftw, FFTW_ESTIMATE);

	fftw_execute(p);

	for (l = 0; l < (n/2+1); l++)
	{
		out[l].r = out_fftw[l][0];
		out[l].i = out_fftw[l][1];
	}

	fftw_destroy_plan(p);
	fftw_free(out_fftw);
} /* fftw() */

#else

int
calc_fft(double *data, long n, complex *out)
{
	int ret;
	long l;
	double *data_real, *data_complex;

	ret = 0;

	data_real = (double *)calloc(n, sizeof(double));
	data_complex = (double *)calloc(n, sizeof(double));

	for (l = 0; l < n; l++)
		data_real[l] = data[l];

	ret = fft_sing(data_real, data_complex, n, n, n, 1);

	for (l = 0; l < (n/2+1); l++)
	{
		out[l].r = data_real[l];
		out[l].i = data_complex[l];
	}

	free(data_real);
	free(data_complex);

	return ret;
} /* calc_fft() */

#endif
