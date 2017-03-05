/*----------------------------------------------------------------------------
 * fft.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/process/freq_analysis/fft.h,v 1.1 2003/09/10 13:14:57 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef FFT_H
#define FFT_H


#ifndef PI
#define PI 3.14159265358979323846
#endif /* PI */

typedef struct
{
	double r, i;
}
complex;


void complex_exp(complex * r, complex * z);
void r_cnjg(complex * r, complex * z);
void spfftc(complex * x, long n, long isign);
void spfftr(complex * x, long n, long inverse);


#endif /* FFT_H */
