/*----------------------------------------------------------------------------
 * simple.h
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _SIMPLE_H
#define _SIMPLE_H

#include <ra.h>
#include <ra_priv.h>


struct detect_info
{
	meas_handle mh;
	rec_handle rh;
};				/* struct detect_info */


struct events
{
	struct events *next;
	struct events *prev;

	long pos;		/* position of beat */
	int width;		/* width of qrs-complex */

	/* positons in "QRS-complex" (offset from pos in sample units) */
	int num_morph_channels;
}; /* struct events */


struct detect_result
{
	int num_events;		/* num of detected events */
	struct events *ev;	/* detected events */
}; /* struct detect_result */


int simple_detect(struct proc_info *pi, struct detect_result *dr);
long search_beat(rec_handle rh, int ch, long pos, double samplerate, int len);


/* ------------------------------ internal ------------------------------ */
#define BUFFER_SIZE  100000
#define BEAT_COMBINE_RANGE  10	/* 10 sec */


struct derivative_info
{
	/* last two values before calc derivative */
	double last1;
	double last2;

	/* last two values after calc derivative */
	double der1;
	double der2;
};				/* struct derivative_info */

#ifdef WIN32
#define inline  __inline
#endif /* WIN32 */

inline double boxcar_filter(double *buf, int size, int *pos, double value);
inline double derivative(struct derivative_info *der_i, double v);

int ch_detect(struct proc_info *pi, int ch, int *n, struct events **bhead);
int get_orientation(double *buf, int size, int *minmax_pos);
struct events *combine_events(struct proc_info *pi, struct events **b);

int save_detect_result(struct proc_info *pi, struct detect_result *dr, struct events *all);


#endif /* _SIMPLE_H */
