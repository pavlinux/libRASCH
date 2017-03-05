/*----------------------------------------------------------------------------
 * frequency.h
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/process/hrv/frequency.h,v 1.3 2003/04/08 13:08:02 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef FREQUENCY_H
#define FREQUENCY_H


/*#define EQUIDIST_MS   292*/
#define PI 3.14159265358979323846

typedef struct
{
	double r, i;
}
complex;


void complex_exp(complex * r, complex * z);
void r_cnjg(complex * r, complex * z);
void spfftc(complex * x, long n, long isign);
void spfftr(complex * x, long n, long inverse);

int make_equidistant(double *v, double *pos, long n, double distance, double **equi, long *n_equi);
int make_instantaneous_period(double *v, double *pos, long n, double distance, double **equi, long *n_equi);

int remove_trend(double *v, long n);
int remove_mean(double *v, long n);

long calc_idx_from_freq(double samplefreq, long n_sample, double freq);
int calc_periodogram(double *v, long n, double samplefreq, double **spec, long *n_spec, char *window);
int calc_welch_spec(double *v, long n, double samplefreq, double **spec, long *n_spec, long len_ms,
		    long overlap, char *window);
int calc_log_freq_smoothed_spec(double *spec, long n, double samplerate, double **smoothed, long *n_sm);

int window_data(double *d, long n, char *window, double *w_power);
void window_hanning(double *v, long n, double *power);
void window_hamming(double *v, long n, double *power);
void window_bartlett(double *v, long n, double *power);
void window_blackman(double *v, long n, double *power);

#endif /* FREQUENCY_H */
