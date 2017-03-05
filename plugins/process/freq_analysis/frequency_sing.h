/*----------------------------------------------------------------------------
 * frequency_sing.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef FREQUENCY_H
#define FREQUENCY_H

#include "fft.h"		/* needed for complex data struct */

int make_equidistant(double *v, double *pos, long n, double distance, double **equi, long *n_equi);
int make_instantaneous_period(double *v, double *pos, long n, double distance, double **equi, long *n_equi);

int remove_trend(double *v, long n);
int remove_mean(double *v, long n);

long calc_idx_from_freq(double samplefreq, long n_sample, double freq);
int calc_periodogram(double *v, long n, double samplefreq, double **spec, long *n_spec, int rm_mean, char *window);
int calc_welch_spec(double *v, long n, double samplefreq, double **spec, long *n_spec, long len_ms,
		    long overlap, int rm_mean, char *window);
int calc_log_freq_smoothed_spec(const double *spec, long n, double samplerate, double **smoothed, long *n_sm);

int window_data(double *d, long n, char *window, double *w_power);
void window_hanning(double *v, long n, double *power);
void window_hamming(double *v, long n, double *power);
void window_bartlett(double *v, long n, double *power);
void window_blackman(double *v, long n, double *power);

int calc_fft(double *data, long n, complex *out);


#endif /* FREQUENCY_H */
