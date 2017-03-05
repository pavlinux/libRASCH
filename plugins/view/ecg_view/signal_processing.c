/*----------------------------------------------------------------------------
 * signal_processing.c  -  provides several DSP routines
 *
 * Description:
 * The file provides the following digital signal processing (DSP) routines:
 *  - FIR filter creation
 *  - filtfilt function to filter a signal without a phase-distortion
 *  - interpolation (increase in samplerate) of a signal
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id: $
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "signal_processing.h"

/*---------------------------------------------------------------*/
/* Design FIR filter using window method. Hamming window is used */
/* If sucess, return a point to the filter coefficient array,    */
/* otherwise, return NULL. Calling program should release the    */
/* allocated memory in this subroutine                           */
/*                                                               */
/*                                                               */
/*  Suppose sampling rate is 2 Hz                                */
/*                                                               */
/*  Len : filter length, should be ODD and Len>=3                */
/*  CutLow : low cutoff, when lowpass, CutLow = 0.0              */
/*  CutHigh: high cutoff, when highpass, CutHigh = 1.0           */
/*  when bandpass,    0.0 < CutLow < CutHigh < 1.0               */
/*                                                               */
/*  example:                                                     */
/*      Coef = fir_dsgn(127, 0.3, 0.8);                          */
/*   return a bandpass filter                                    */
/*---------------------------------------------------------------*/
int
fir_filter_design(long order, double cut_low, double cut_high, double **b, long *num)
{
	double sum, temp;
	int coef_num, half_len, i;
	
	/*---------------------------------------------*/
	/* adjust the number of coefficients to be ODD */
	/*---------------------------------------------*/
	coef_num = order;
	if (order % 2 == 0) {
		coef_num++;
	}
	*num = coef_num;
	half_len = (coef_num - 1) / 2;
	
	/*----------------------------------------------------*/
	/* Allocate memory for coefficients if length changed */
	/*----------------------------------------------------*/
	if (b == NULL)
		return -1;
	*b = malloc(sizeof(double) * coef_num);
	
	/*----------------*/
	/* Lowpass filter */
	/*----------------*/
	if ((cut_low == 0.0) && (cut_high < 1.0))
	{		
		(*b)[half_len] = cut_high;
		for (i = 1; i <= half_len; i++)
		{
			temp = PI * i;
			(*b)[half_len + i] = sin(cut_high * temp) / temp;
			(*b)[half_len - i] = (*b)[half_len + i];
		}
		
		/*---------------------------*/
		/* multiplying with a window */
		/*---------------------------*/
		temp = 2.0 * PI / (coef_num - 1.0);
		sum = 0.0;
		for (i = 0; i < coef_num; i++)
		{
			(*b)[i] *= (0.54 - 0.46 * cos(temp * i));
			sum += (*b)[i];
		}
		
		/*---------------------*/
		/* Normalize GAIN to 1 */
		/*---------------------*/
		for (i = 0; i < coef_num; i++)
			(*b)[i] /= fabs(sum);
		
		return 0;		
	}  /* if Lowpass */
	
	
	/*-----------------*/
	/* Highpass filter */
	/*-----------------*/
	if ((cut_low > 0.0) && (cut_high == 1.0))
	{
		(*b)[half_len] = cut_low;
		for (i = 1; i <= half_len; i++)
		{
			temp = PI * i;
			(*b)[half_len + i] = sin(cut_low * temp) / temp;
			(*b)[half_len - i] = (*b)[half_len + i];
		}
		
		/*---------------------------*/
		/* multiplying with a window */
		/*---------------------------*/
		temp = 2.0 * PI / (coef_num - 1.0);
		sum = 0.0;
		for (i = 0; i < coef_num; i++)
		{
			(*b)[i] *= -(0.54 - 0.46 * cos(temp * i));
			if (i % 2 == 0)
				sum += (*b)[i];  /* poly(-1), even +, odd -*/
			else
				sum -= (*b)[i];
		}
		
		(*b)[half_len] += 1;
		sum += 1;
		
		/*---------------------*/
		/* Normalize GAIN to 1 */
		/*---------------------*/
		for (i = 0; i < coef_num; i++)
			(*b)[i] /= fabs(sum);
		
		return 0;		
	} /* if HighPass */
	
	
	/*-----------------*/
	/* Bandpass filter */
	/*-----------------*/
	if ((cut_low > 0.0) && (cut_high < 1.0) && (cut_low < cut_high))
	{		
		(*b)[half_len] = cut_high - cut_low;
		for (i = 1; i <= half_len; i++)
		{
			temp = PI * i;
			(*b)[half_len + i] = 2.0 * sin((cut_high - cut_low) / 2.0*temp) *
				cos((cut_high + cut_low) / 2.0*temp) / temp;
			(*b)[half_len - i] = (*b)[half_len + i];
		}
		
		/*---------------------------*/
		/* multiplying with a window */
		/*---------------------------*/
		temp = 2.0 * PI / (coef_num - 1.0);
		sum = 0.0;
		for (i = 0; i< coef_num; i++)
		{
			(*b)[i] *= (0.54 - 0.46 * cos(temp * i));
			sum += (*b)[i];
		}
		
		/*---------------------*/
		/* Normalize GAIN to 1 */
		/*---------------------*/
		for (i = 0; i < coef_num; i++)
			(*b)[i] /= fabs(sum);

		return 0;
		
	} /* if */
	
	/*-----------------*/
	/* Bandstop filter */
	/*-----------------*/
	if ((cut_low > 0.0) && (cut_high < 1.0) && (cut_low > cut_high))
	{		
		(*b)[half_len] = cut_low - cut_high;
		for (i = 1; i <= half_len; i++)
		{
			temp = PI * i;
			(*b)[half_len + i] = 2.0 * sin((cut_low - cut_high) / 2.0 * temp) *
				cos((cut_high + cut_low) / 2.0 * temp) / temp;
			(*b)[half_len - i] = (*b)[half_len + i];
		}
		
		/*---------------------------*/
		/* multiplying with a window */
		/*---------------------------*/
		temp = 2.0 * PI / (coef_num - 1.0);
		sum = 0.0;
		for (i = 0; i < coef_num; i++)
		{
			(*b)[i] *= -(0.54 - 0.46 * cos(temp * i));
			sum += (*b)[i];
		}
		
		(*b)[half_len] += 1;
		sum += 1;
		
		/*---------------------*/
		/* Normalize GAIN to 1 */
		/*---------------------*/
		for (i = 0; i < coef_num; i++)
			(*b)[i] /= fabs(sum);

		return 0;		
	}
	
	return -1;
} /* fir_filter_design() */


int
filter(double *b, double *a, long order, double *data, long num_samples, int direction)
{
	long l, m, curr, add;
	double *xv, *yv;
	double sum_a, sum_b;

	xv = calloc(order, sizeof(double));
	yv = calloc(order, sizeof(double));

	if (direction == 0)
	{
		for (l = 0; l < order; l++)
			xv[l] = yv[l] = data[0];

		curr = 0;
		add = 1;
	}
	else
	{
		for (l = 0; l < order; l++)
			xv[l] = yv[l] = data[num_samples-1];

		curr = num_samples - 1;
		add = -1;
	}

	
	for (l = 0; l < num_samples; l++)
	{
		for (m = order-1; m > 0; m--)
		{
			xv[m] = xv[m-1];
			yv[m] = yv[m-1];
		}
		xv[0] = data[curr];

		sum_a = sum_b = 0.0;
		for (m = 0; m < order; m++)
		{
			if (a && (m > 0))
				sum_a += (yv[m] * a[m]);
			sum_b += (xv[m] * b[m]);
		}
		yv[0] = sum_a + sum_b;

		data[curr] = yv[0];

		curr += add;
	}

	if (xv)
		free(xv);
	if (yv)
		free(yv);

	return 0;
} /* filter() */


int
filtfilt(double *b, double *a, long order, double *data, long num_samples)
{
	int ret;

	if ((ret = filter(b, a, order, data, num_samples, 0)) != 0)
		return ret;

	if ((ret = filter(b, a, order, data, num_samples, 1)) != 0)
		return ret;

	return 0;
} /* filtfilt() */


int
interp(double *in, double *out, long num_samples, long factor)
{
	int ret;
	long l;
	double *b = NULL;
	long num = 0;

	if ((in == NULL) || (out == NULL) || (num_samples <= 0) || (factor <= 0))
		return -1;
	if (factor == 1)
	{
		memcpy(out, in, sizeof(double) * num_samples);
		return 0;
	}

	ret = fir_filter_design(2*factor+1, 0.0, 0.5/factor, &b, &num);
	if (ret != 0)
		return ret;

	memset(out, 0, sizeof(double) * num_samples * factor);
	for (l = 0; l < num_samples; l++)
		out[l*factor] = (double)factor * in[l];
	
 	ret = filtfilt(b, NULL, num, out, (num_samples * factor));

	free(b);

	return ret;
} /* interp() */
