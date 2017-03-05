/*----------------------------------------------------------------------------
 * statistics.h
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

#ifndef STATISTICS_H
#define STATISTICS_H

double get_min(double *values, long num, long *pos);
double get_max(double *values, long num, long *pos);
double mean(double *values, long num);
double variance(double *values, long num);
double stddev(double *values, long num);
int histogram(double *values, long num, long **hist, long *hist_start, long *hist_size, int bin);
int calc_regression(double *x, double *y, long n, double *offset, double *slope);

#endif /* STATISTICS_H */
