/*----------------------------------------------------------------------------
 * statistics.c
 *
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/process/hrv/statistics.c,v 1.2 2003/03/21 09:43:10 rasch Exp $
 *--------------------------------------------------------------------------*/

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "statistics.h"


double
get_min(double *values, long num, long *pos)
{
	long l, p;
	double min;

	min = values[0];
	p = 0;
	for (l = 1; l < num; l++)
	{
		if (values[l] < min)
		{
			min = values[l];
			p = l;
		}
	}
	if (pos)
		*pos = p;

	return min;
}				/* get_min() */


double
get_max(double *values, long num, long *pos)
{
	long l, p;
	double max;

	max = values[0];
	p = 0;
	for (l = 1; l < num; l++)
	{
		if (values[l] > max)
		{
			max = values[l];
			p = l;
		}
	}
	if (pos)
		*pos = p;

	return max;
}				/* get_max() */


double
mean(double *values, long num)
{
	long l;
	double mean = 0.0;

	for (l = 0; l < num; l++)
		mean += values[l];
	mean /= (double) num;

	return mean;
}				/* mean() */


double
variance(double *values, long num)
{
	long l;
	double s = 0.0;
	double m = mean(values, num);

	for (l = 0; l < num; l++)
	{
		double t = values[l] - m;
		s += (t * t);
	}
	s /= (double) num;

	return s;
}				/* variance() */


double
stddev(double *values, long num)
{
	return sqrt(variance(values, num));
}				/* stddev() */


int
histogram(double *values, long num, long **hist, long *hist_start, long *hist_size, int bin)
{
	long l, min, max;

	if (bin <= 0)
		return -1;

	min = (long) (get_min(values, num, NULL));
	min -= (abs(min) % bin);
	max = (long) (get_max(values, num, NULL));
	max += (abs(max) % bin);

	*hist = NULL;
	*hist_size = (max - min) / bin;
	if (*hist_size <= 0)
		return 0;
	(*hist_size)++;

	*hist_start = min;
	*hist = calloc(sizeof(long), (*hist_size));

	for (l = 0; l < num; l++)
	{
		long idx = ((long) values[l] - min);
		idx /= bin;
		if ((idx < 0) || (idx >= *hist_size))
			assert(0);
		else
			(*hist)[idx]++;
	}

	return 0;
}				/* histogram */


int
calc_regression(double *x, double *y, long n, double *offset, double *slope)
{
	double mean_x, mean_y;
	double sum_sq_x, sum_xy;
	long l;

	if (n < 2)
	{
		*offset = 0;
		*slope = 0;
		return -1;
	}

	mean_x = mean_y = sum_sq_x = sum_xy = 0.0;
	for (l = 0; l < n; l++)
	{
		mean_x += x[l];
		sum_sq_x += (x[l] * x[l]);
		mean_y += y[l];
		sum_xy += (x[l] * y[l]);
	}
	mean_x /= (double) n;
	mean_y /= (double) n;

	*slope = (sum_xy - ((double) n * mean_x * mean_y));
	*slope /= (sum_sq_x - ((double) n * mean_x * mean_x));

	*offset = mean_y - (*slope * mean_x);

	return 0;
}				/* calc_regression() */
