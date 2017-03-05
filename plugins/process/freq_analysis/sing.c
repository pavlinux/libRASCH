/**************************************************************************

	Javier Soley, Ph. D,   FJSOLEY @UCRVM2.BITNET
	Escuela de F¡sica y Centro de Investigaciones Geof¡sicas
	Universidad de Costa Rica

***************************************************************************/

/*	Computes the DISCRETE FOURIER TRANSFORM of very long data series.
 *   Restriction: the data has to fit in conventional memory.
 *
 *   Compile in compact or large models ===> Pointers must be FAR
 *   Two huge pointers are used to access the real and imaginary
 *   parts of the transform without 64k wrap around. 
 *
 *   This functions are translations from the fortran program in
 *
 *   R. C. Singleton, An algorithm for computing the mixed radix fast
 *   Fourier transform 
 *
 *	IEEE Trans. Audio Electroacoust., vol. AU-17, pp. 93-10, June 1969.
 *   Some features are:
 *
 *		1-) Accepts an order of transform that can be factored not only
 *		    in prime factors such 2 and 4, but also including odd factors
 *		    as 3, 5, 7, 11, etc.
 *		2-) Generates sines and cosines recursively and includes
 *		    corrections for truncation errors.
 *		3-) The original subroutine accepts multivariate data. This 
 *			translation does not implement that option (because I
 *			do not needed right now).
 *
 *	Singleton wrote his subroutine in Fortran and in such a way that it 
 *	could be ported allmost directly to assembly language. I transcribed
 *   it to C with little effort to make it structured. So I apologize to
 *   all those C purists out there!!!!!!!!
 *
 * 
 *  Changes by Raphael Schneider <rasch@med1.med.tum.de>
 *  - 18.10.2004:
 *    + replaced the fixed arrays with dynamic memory allocation
 *      (Because I do not understand the algorithm, I hope that I
 *      did it right. But I think that I allocated more than needed,
 *      therefore it should work.)
 *    + cosmetic changes
 * 
 * $Id$
 */

				/*  Version 2.0 March/30/92 */
/* Includes */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#endif

/* Defines */
#define	TWO_PI	((double)2.0 * M_PI)


/* Globals */
long nn;
long m; /* number of factors (size of nfac) */
long flag, jf, jc, kspan, ks, kt, nt, kk, i;
double c72, s72, s120, cd, sd, rad, radf;
double *at, *bt;
long *nfac;   
long *np;
int inc;


/* The functions */
void
radix_2(double *a, double *b)
{
	long k1, k2;
	double ak, bk, c1, s1;

	kspan >>= 1;
	k1 = kspan +2;

	do
	{
		do
		{
			k2 = kk + kspan;
			ak = a[k2-1];
			bk = b[k2-1];
			a[k2-1] = a[kk-1] -ak;
			b[k2-1] = b[kk-1] -bk;
			a[kk-1] += ak;
			b[kk-1] += bk;
			kk = k2 + kspan;
		} while ( kk <= nn);
		kk = kk - nn;
	} while ( kk <= jc);

	if ( kk > kspan)
		flag = 1; 
	else
	{
		do
		{
			c1 = 1.0 - cd;
			s1 = sd;
			do
			{
				do
				{
					do
					{
						k2 = kk + kspan;
						ak = a[kk-1]- a[k2-1];
						bk = b[kk-1]- b[k2-1];
						a[kk-1] += a[k2-1];
						b[kk-1] += b[k2-1];
						a[k2-1]  = c1*ak - s1*bk;
						b[k2-1]  = s1*ak + c1*bk;
						kk = k2 + kspan;
					} while ( kk < nt );
					k2 = kk - nt;
					c1 = -c1;
					kk = k1 - k2;
				} while ( kk > k2 );
				ak = c1- (cd*c1+sd*s1);
				s1 = (sd*c1-cd*s1) +s1;
				
				/***** Compensate for truncation errors   *****/

				c1 = 0.5/(ak*ak+s1*s1)+0.5;
				s1 *= c1;
				c1 *= ak;
				kk += jc;
			} while ( kk < k2);
			k1 = k1 + inc + inc;
			kk = (k1- kspan) /2 + jc;
		} while ( kk <= jc + jc );
	}
} /* radix_2() */


void
radix_4(int isn, double *a, double *b)
{
	long k1, k2, k3;
	double akp, akm, ajm, ajp, bkm, bkp, bjm, bjp;
	double  c1, s1, c2, s2, c3, s3;
	kspan /= 4;

	c2 = s2 = c3 = s3 = 0.0;

cuatro_1: 
	c1 = 1.0;
	s1 = 0;
	do
	{
		do
		{
			do
			{
				k1  =  kk + kspan;
				k2  =  k1 + kspan;
				k3  =  k2 + kspan;
				akp =  a[kk-1] + a[k2-1];
				akm =  a[kk-1] - a[k2-1];
				ajp =  a[ k1-1] + a[k3-1];
				ajm =  a[ k1-1] - a[k3-1];
				a[kk-1] = akp + ajp;
				ajp = akp - ajp;
				bkp = b[kk-1] + b[k2-1];
				bkm = b[kk-1] - b[k2-1];
				bjp = b[k1-1] + b[k3-1];
				bjm = b[k1-1] - b[k3-1];
				b[kk-1] = bkp + bjp;
				bjp = bkp - bjp;
				if ( isn < 0)
					goto cuatro_5;
				akp = akm - bjm;
				akm = akm + bjm;
				bkp = bkm + ajm;
				bkm = bkm - ajm;
				if (s1 == 0.0)
					goto cuatro_6;
cuatro_3:
				a[ k1-1] = akp*c1 - bkp*s1;
				b[ k1-1] = akp*s1 + bkp*c1;
				a[ k2-1] = ajp*c2 - bjp*s2;
				b[ k2-1] = ajp*s2 + bjp*c2;
				a[ k3-1] = akm*c3 - bkm*s3;
				b[ k3-1] = akm*s3 + bkm*c3;
				kk = k3 + kspan;
			}  while ( kk <= nt);
				
cuatro_4: 
			c2 = c1 - (cd*c1 + sd*s1);
			s1 = (sd*c1 - cd*s1) + s1;

			/***** Compensate for truncation errors *****/

			c1 = 0.5 / (c2*c2 + s1*s1) +0.5;
			s1 = c1 * s1;
			c1 = c1 * c2;
			c2 = c1*c1 - s1*s1;
			s2 = 2.0 * c1 *s1;
			c3 = c2*c1 - s2*s1;
			s3 = c2*s1 + s2*c1;
			kk = kk -nt + jc;
		} while ( kk <= kspan);
		
		kk = kk - kspan + inc;
		if ( kk <= jc)
			goto cuatro_1;
		if ( kspan == jc)
			flag =1;
		goto out;
cuatro_5:
		akp = akm + bjm;
		akm = akm - bjm;
		bkp = bkm - ajm;
		bkm = bkm + ajm;
		if (s1 != 0.0)
			goto cuatro_3;
cuatro_6:
		a[k1-1] = akp;
		b[k1-1] = bkp;
		b[k2-1] = bjp;
		a[k2-1] = ajp;
		a[k3-1] = akm;
		b[k3-1] = bkm;
		kk = k3 + kspan;
	} while ( kk <= nt);
	goto cuatro_4;
out: 
	s1 = s1 + 0.0;
} /* radix_4() */


/* Find prime factors of n */
void
fac_des(long n)
{
	long k, j, jj;
	k = n;
	m = 0;

	while (k-(k / 16)*16 == 0)
	{
		m++;
		nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
		nfac[m-1] = 4;
		k /= 16;
	} 
	j  = 3;
	jj = 9;
	do
	{
		while (k % jj == 0)
		{
			m++;
			nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
			nfac[m-1] = j;
			k /= jj;
		}
		j += 2;
		jj = j * j;
	} while (jj <= k);
	if (k <= 4)
	{
		kt = m;
		nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
		nfac[m] = k;
		if (k != 1)
			m++;
	}
	else
	{
		if (k-(k / 4)*4 == 0)
		{
			m++;
			nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
			nfac[m-1] = 2;
			k /= 4;
		}
		kt = m;
		j = 2;
		do
		{
			if (k % j == 0)
			{
				m++;
				nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
				nfac[m-1] = j;
				k /= j;
			}
			j = ((j+1)/ 2)*2 + 1;
		} while (j <= k);
	}
	if (kt != 0)
	{
		j = kt;
		do
		{
			m++;
			nfac = (long *)realloc(nfac, sizeof(long) * (m+1));
			nfac[m-1] = nfac[j-1];
			j--;
		} while (j != 0);
	}
} /* fac_des() */


/* Permute the results to normal order  */
int
permute(long ntot, long n, double *a, double *b, long maxf)
{
	long  k, j, k1, k2, k3, kspnn;
	double ak, bk;
	long  ii, jj;

	k3 = 0;

	np[0] = ks;

	if (kt != 0)
	{
		k = kt + kt + 1;
		if (m < k)
			k--;
		j = 1;
		np[k] = jc;
		do
		{
			np[j] = np[j-1] / nfac[j-1];
			np[k-1] = np[k] * nfac[j-1];
			j++;
			k--;
		} while (j < k);
		k3 = np[k];   
		kspan = np[1];
		kk = jc+1;
		k2 = kspan + 1;  
		j = 1;

		/***** Permutation of one dimensional transform *****/

		if (n == ntot)
		{
			do
			{
				do
				{
					ak = a[kk-1];
					a[kk-1] = a[k2-1];
					a[k2-1] = ak;
					bk = b[kk-1];
					b[kk-1] = b[k2-1];
					b[k2-1] = bk;
					kk += inc;
					k2 += kspan;
				} while ( k2 < ks);
ocho_30:
				do
				{
					k2 -= np[j-1];
					j++;
					k2 += np[j];
				} while (k2 > np[j-1]);
				j = 1;
ocho_40:  		
				j = j + 0;
			} while (kk < k2);
			kk += inc;
			k2 += kspan;
			if (k2 < ks)
				goto ocho_40;
			if (kk < ks)
				goto ocho_30;
			jc = k3;
		}
		else
		{        /* Permutation for multiple transform  */
ocho_50:
			do
			{
				do
				{
					k = kk + jc;
					do
					{
						ak = a[kk-1];	
						a[kk-1] = a[k2-1];
						a[k2-1] = ak;							
						bk = b[kk-1];
						b[kk-1] = b[k2-1];
						b[k2-1] = bk;
						kk += inc;
						k2 += inc;
					} while ( kk < k);
					kk = kk + ks - jc;
					k2 = k2 + ks - jc;
				} while (kk < nt);
				k2 = k2 - nt + kspan;
				kk = kk - nt + jc;
			} while (k2 < ks);
			do
			{
				do
				{
					k2 -= np[j-1];
					j++;
					k2 += np[j];
				} while (k2 > np[j-1]);
				j =1;
				do
				{
					if ( kk < k2 )
						goto ocho_50;
					kk +=jc;
					k2 += kspan;
				} while (k2 < ks);
			} while (kk < ks);
			jc = k3;
		}
	}
	
	if ( (2*kt +1) < m)
	{
		kspnn = np[kt];
		/* Permutation of square-free factors of n */
		j = m - kt;
		nfac[j] = 1;
		do
		{
			nfac[j-1] *= nfac[j];
			j--;
		} while (j != kt);
		kt++;
		nn = nfac[kt-1] -1;
		jj =0;
		j=0;
		goto nueve_06;
nueve_02:
		jj -= k2;
		k2 = kk;
		k++;
		kk = nfac[k-1];
		do
		{
			jj += kk;
			if ( jj >= k2 )
				goto nueve_02;
			np[j-1] = jj;
nueve_06:
			k2 = nfac[kt-1];
			k = kt+1;
			kk = nfac[k-1];
			j++;
		} while (j <= nn);
		  /* determine the permutation cycles  of length greater then 1 */
		j =0;
		goto nueve_14;
		do
		{
			do
			{		
				k = kk;
				kk = np[k-1];
				np[k-1] = -kk;
			} while ( kk != j);
			k3 = kk;
nueve_14:
			do
			{
				j++;
				kk = np[j-1];
			} while (kk <0);
		} while ( kk != j);
		np[j-1] = -j;
		if (j != nn)
			goto nueve_14;
		maxf *= inc;				
		/* Reorder a and b following the permutation cycles */
		goto nueve_50;
		do
		{
			do
			{
				do { j--;} while (np[j-1] <0);
				jj = jc;
				do
				{
					kspan = jj;
					if ( jj > maxf)
						kspan = maxf;
					jj -= kspan;
					k = np[j-1];	
					kk = jc*k + ii + jj;
					k1 = kk + kspan;
					k2 =0;
					do
					{
						k2++;
						at[k2-1] = a[k1-1];
						bt[k2-1] = b[k1-1];
						k1 -= inc;
					} while (k1 != kk);
					do
					{
						k1 = kk + kspan;
						k2 = k1 - jc*(k + np[k-1]);
						k = -np[k-1];
						do
						{
							a[k1-1] = a[k2-1];
							b[k1-1] = b[k2-1];
							k1 -= inc;
							k2 -= inc;
						} while (k1 != kk);
						kk = k2;
					} while ( k != j);
					k1 = kk + kspan;
					k2 = 0;
					do
					{
						k2++;
						a[k1-1] = at[k2-1];
						b[k1-1] = bt[k2-1];
						k1 -= inc;
					} while ( k1 != kk);							
				} while ( jj != 0 );
			} while (j !=1);
nueve_50:
			j = k3+1;
			nt -= kspnn;
			ii = nt - inc +1;
		} while ( nt >= 0);
		k = k + 0;
	}

	return 0;
} /* permute() */


/**************************************************************************
Functions for prime factor radix
***************************************************************************/
void
radix_3(double *a, double *b)
{
	long k1, k2;
	double ak, bk, aj, bj;

	do
	{
		do
		{
			k1 = kk + kspan;
			k2 = k1+ kspan;
			ak = a[kk-1];
			bk = b[kk-1];
			aj = a[k1-1] + a[k2-1];
			bj = b[k1-1] + b[k2-1];
			a[kk-1] = ak + aj;
			b[kk-1] = bk + bj;
			ak = -0.5*aj + ak;
			bk = -0.5*bj + bk;
			aj = (a[k1-1]-a[k2-1])*s120;
			bj = (b[k1-1]-b[k2-1])*s120;
			a[k1-1] = ak - bj;
			b[k1-1] = bk + aj;
			a[k2-1] = ak + bj;
			b[k2-1] = bk - aj;
			kk = k2 + kspan;
		} while ( kk < nn);
		kk = kk - nn;
	} while ( kk <= kspan);
} /* radix_3() */
	

void
radix_5(double *a, double *b)
{
	long k1, k2, k3, k4;
	double ak, aj, bk, bj, akp, akm, ajm, ajp, aa, bkp, bkm, bjm, bjp, bb;
	double c2, s2;

	c2 = c72*c72 - s72*s72;
	s2 = 2 * c72 * s72;
	do
	{
		do
		{
			k1 = kk + kspan;
			k2 = k1 + kspan;
			k3 = k2 + kspan;
			k4 = k3 + kspan;
			akp = a[k1-1] + a[k4-1];
			akm = a[k1-1] - a[k4-1];
			bkp = b[k1-1] + b[k4-1];
			bkm = b[k1-1] - b[k4-1];
			ajp = a[k2-1] + a[k3-1];
			ajm = a[k2-1] - a[k3-1];
			bjp = b[k2-1] + b[k3-1];
			bjm = b[k2-1] - b[k3-1];
			aa  = a[kk-1];
			bb  = b[kk-1];
			a[kk-1] = aa + akp + ajp;
			b[kk-1] = bb + bkp + bjp;
			ak = akp*c72 + ajp*c2 + aa;
			bk = bkp*c72 + bjp*c2 + bb;
			aj = akm*s72 + ajm*s2;
			bj = bkm*s72 + bjm*s2;
			a[k1-1] = ak - bj;
			a[k4-1] = ak + bj;
			b[k1-1] = bk + aj;
			b[k4-1] = bk - aj;
			ak = akp*c2 + ajp*c72 + aa;
			bk = bkp*c2 + bjp*c72 + bb;
			aj = akm*s2 - ajm*s72;
			bj = bkm*s2 - bjm*s72;
			a[k2-1] = ak - bj;
			a[k3-1] = ak + bj;
			b[k2-1] = bk + aj;
			b[k3-1] = bk - aj;
			kk = k4 + kspan;
		} while ( kk < nn);
		kk -= nn;
	} while ( kk <= kspan);
} /* radix_5() */


void
fac_imp(double *a, double *b, long max_factor)
{    
	long k, kspnn, j, k1, k2, jj;
	double ak, bk, aa, bb, aj, bj;
	double c1, s1, c2, s2;
	double *ck, *sk;

	ck = (double *)malloc(sizeof(double) * max_factor);
	sk = (double *)malloc(sizeof(double) * max_factor);

	k = nfac[i-1];
	kspnn = kspan;
	kspan /= k;
	if (k == 3)
		radix_3(a, b);
	if (k == 5)
		radix_5(a, b);
	if ((k == 3) || (k == 5))
		goto twi;

	if (k != jf)
	{
		jf = k;
		s1 = rad/k;
		c1 = cos(s1);
		s1 = sin(s1);
		ck[jf-1] = 1.0;
		sk[jf-1] = 0.0;
		j = 1;
		do
		{
			ck[j-1] = ck[k-1]*c1 + sk[k-1]*s1;
			sk[j-1] = ck[k-1]*s1 - sk[k-1]*c1;
			k--;
			ck[k-1] =  ck[j-1];
			sk[k-1] = -sk[j-1];
			j++;
		} while (j < k);
	}
	do
	{
		do
		{
			k1 = kk;
			k2 = kk + kspnn;
			aa = a[kk-1];
			bb = b[kk-1];
			ak = aa;
			bk = bb;
			j  = 1;
			k1 = k1 + kspan;
			do
			{
				k2 -= kspan;
				j++;
				at[j-1] = a[k1-1] + a[k2-1];
				ak += at[j-1];
				bt[j-1] = b[k1-1] + b[k2-1];
				bk += bt[j-1];
				j++;
				at[j-1]  = a[k1-1] - a[k2-1];
				bt[j-1] = b[k1-1] - b[k2-1];
				k1 += kspan;
			} while ( k1 < k2);
			a[kk-1] = ak;
			b[kk-1] = bk;
			k1 = kk;
			k2 = kk + kspnn;
			j = 1;
			do
			{
				k1 += kspan;
				k2 -= kspan;
				jj = j; 
				ak = aa;
				bk = bb;
				aj = 0.0;
				bj = 0.0;
				k = 1;
				do
				{
					k++;
					ak = at[k-1]*ck[jj-1] + ak;
					bk = bt[k-1]*ck[jj-1] + bk;
					k++;
					aj = at[k-1]*sk[jj-1] + aj;
					bj = bt[k-1]*sk[jj-1] + bj;
					jj += j;
					if (jj>jf)
						jj-=jf;
				} while ( k<jf);
				k = jf - j;
				a[k1-1] = ak - bj;
				b[k1-1] = bk + aj;
				a[k2-1] = ak + bj;
				b[k2-1] = bk - aj;
				j++;
			} while (j<k);
			kk += kspnn;
		} while (kk <= nn);
		kk -= nn;
	} while ( kk <= kspan);

        /***** Multiply by twiddle factors  *****/

twi:
	if (i==m)
		flag = 1;
	else
	{
		kk = jc + 1;
		do
		{
			c2 = 1.0 - cd;
			s1 = sd;
			do
			{
				c1  = c2;
				s2  = s1;
				kk += kspan;
				do
				{
					do
					{
						ak = a[kk-1];
						a[kk-1] = c2*ak - s2*b[kk-1];
						b[kk-1] = s2*ak + c2*b[kk-1];
						kk += kspnn;
					} while ( kk <= nt);
					ak = s1 * s2;
					s2 = s1*c2 + c1*s2;
					c2 = c1*c2 - ak;
					kk = kk - nt + kspan;
				} while ( kk <= kspnn);
				c2 = c1 - (cd*c1 + sd*s1);
				s1 = s1 + (sd*c1 - cd*s1);
				
				/***** Compensate for truncation errors *****/

				c1 = 0.5/(c2*c2 + s1*s1) + 0.5;
				s1 *= c1;
				c2 *= c1;
				kk  = kk - kspnn + jc;
			} while ( kk <= kspan);		  
			kk = kk - kspan + jc + inc;
		} while ( kk <= (jc+jc));	  

	} 

	/* clean up */
	free(ck);
	free(sk);
} /* fac_imp() */


int
fft_sing(double *a, double *b, long ntot, long n, long nspan, int isn)
{
	int ret = 0;
	long max_factor = 0;
	long l;

	if ( n < 2 )
		return -1;

	nfac = NULL;

	inc  = isn;
	rad  = TWO_PI;
	s72  = rad / 5.0;
	c72  = cos(s72);
	s72  = sin(s72);
	s120 = sqrt(0.75);
	if (isn < 0)
	{ 
		s72  = -s72;
		s120 = -s120;
		rad  = -rad;
		inc  = -inc;
	} 
	nt    = inc * ntot;
	ks    = inc * nspan;
	kspan = ks;
	nn    = nt - inc;
	jc    = ks / n;
	radf  = rad * jc * 0.5;
	i     = 0;
	jf    = 0;
	flag  = 0;
	fac_des (n);

	/* get max factor */
	for (l = 0; l < m; l++)
	{
		if (nfac[l] > max_factor)
			max_factor = nfac[l];
	}
	
	/* malloc mem for global arrays */
	at = (double *)malloc(sizeof(double) * max_factor);
	bt = (double *)malloc(sizeof(double) * max_factor);
	np = (long *)malloc(sizeof(long) * n);

	do
	{
		sd = radf / kspan;
		cd = 2.0 * sin(sd) * sin(sd);
		sd = sin(sd+sd);
		kk = 1;
		i  = i + 1;
		if (nfac[i-1] == 2)
			radix_2( a, b);
		if (nfac[i-1] == 4) 
			radix_4(isn, a, b); 
		if ( (nfac[i-1] != 2) && (nfac[i-1] != 4)) 
			fac_imp(a, b, max_factor);	
	} while (flag != 1);
	ret = permute(ntot, n, a, b, max_factor);

	/* clean up */
	free(nfac);
	free(at);
	free(bt);
	free(np);

	return ret;
} 

/* Calculates the the Fourier transform of 2*half_length real values.
   Original data values are stored alternately in arrays a and b.
   The cosine coefficients are in a[0], a[1] .........a[half_length} and
   the sine coefficients are in b[0], b[1] .........b[half_length}.
   The coeffcients must be scaled by 1/(4*half_length) in the calling
   procedure.  */

/* April/1/92 Tried Singleton's subroutine and it does not seem to work.
   I am modifying it and folowing the procedure of Cooley, Lewis and Welch
   J. Sound Vib., vol. 12, pp 315-337, July  1970. Will extend the 
   procedure so half_length can be odd also. */	

 /*	Assume we have the transform A(n) of x(even) + i x(odd)
	1-)  A1(n) = (1/2) [ Ac(-n) + A(n)] =(1/2) [Ac(N-n) + A(n)]  
		(Ac = complex of A)    (for N even or odd)
	2-)  A2(n) = (i/2)[Ac(-n)-A(n)] =(i/2) [Ac(N-n)-A(n)]  
		(for N even or odd)
	3-)  C(n) = (1/2)[A1(n) + A2(n)*W2N**(-n)]   (1)
		0,1,2,3..... N   N even
		0,1,2,3..... N-1 N odd						    
		Use the simmetry of the A1 and A2 sequences
		C(N-n) = (1/2) [A1(N-n) + A2(N-n)*W2N**(-N+n)] 
			  = (1/2) [A1c(n) - A2c(n)*W2N**(n)]   (2)
		Evaluate (1) and (2) for n=0,1,2,...N/2 -1 and (1) for N/2
		if N is even  ( (2) is also good
		Evaluate (1) for n =0 and
			    (1) and  (2) for n=1,2,...(N-1)/2  
		if N is odd
				
		Let the factors of two be taken care in the normalization
		outside this procedure, ie, the coefficients will be
		four times larger. */							    

/* 4/april/1992 Everything is working fine. Singleton's Realtr
   works ok. */

void
realtr(double *a, double *b, long half_length, int isn)
{
  	long  nh, j, k;
	double sd, cd, sn, cn, a1r, a1i, a2r, a2i, re, im;
	nh = half_length >> 1;    /* Should work for even and odd */
	sd = M_PI_2 /half_length;
	cd = 2.0 * sin(sd) * sin(sd);
	sd = sin(sd+sd);	
	sn = 0;
	if (isn < 0)
	{
		cn = 1 ;
		a[half_length] = a[0];
		b[half_length] = b[0];  
	}
	else
	{
		cn = -1;
		sd = -sd;
	}

	/* For nh odd the j = nh value is meaningless (and harmless to calculate).
	   Also, the value nh /2 might be calculated twice. */
	for (j=0; j <= nh; j++)
	{
		k = half_length-j;
		a1r = a[j] + a[k];
		a2r = b[j] + b[k];
		a1i = b[j] - b[k];
		a2i = -a[j] + a[k];
		re = cn*a2r + sn*a2i;
		im =-sn*a2r + cn*a2i; 
		b[k] = +im - a1i;  
		b[j] = im + a1i;
		a[k] = a1r - re;
		a[j] = a1r + re;
		a1r = cn - (cd*cn + sd*sn);
		sn = (sd*cn - cd*sn) + sn;
		/* compensate for truncation error */
		cn = 0.5/(a1r*a1r + sn*sn) + 0.5;
		sn *= cn;
		cn *= a1r;
	}
} /* realtr() */







