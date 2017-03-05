/* $Id$ */


#ifndef _SING_H
#define _SING_H

int fft_sing(double *a, double *b, long ntot, long n, long nspan, int isn);
void realtr(double *a, double *b, long half_length, int isn);

#endif  /* _SING_H */

