/*----------------------------------------------------------------------------
 * ra_calibration.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/include/ra_calibration.h,v 1.3 2004/11/25 15:52:51 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifndef RA_CALIBRATION_H
#define RA_CALIBRATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* ---------- defines ---------- */
#define CALIB_TYPE_AUTO     0	/* try to detect calibration type */
#define CALIB_TYPE_PULSE    1	/* calibration signal are pulses */


/* -------------------- structures -------------------- */
struct ra_calibration
{
	rec_handle rh;   /* recording handle */
	long ch;         /* channel of the recording used to measure the calibration signal */
	long start_pos;  /* signal-position to start search for calibration signal [su] */
	long end_pos;    /* signal-position to stop search for calibration signal [su] */

	short use_data;      /* use the data given in 'data' */
	long num_data;      /* number of the data given in 'data' */
	double *data;       /* perform calibration meausure using this data */
	double samplerate;  /* samplerate used for the values in 'data' */

	long type;                  /* type of calibration signal (see CALIB_TYPE_* defines) */
	long difference_lag;        /* gap between samples used for difference signal [su] */
	double min_calib_amp;       /* percentage of maximum difference used as minimal calibration amplitude */
	double segment_length;      /* grid-width used to search for continous calibration signals [seconds] */
	double min_level_length;    /* minimum duration staying on the same level [seconds] */
	double calib_cycle_length;  /* length of one calibration cycle [seconds] */
	short get_cycle_length;      /* flag if the calibration cycle length should be measured */
	long min_cycles;            /* minimum number of calibration cycles required */
}; /* struct ra_calibration */


/* -------------------- globals -------------------- */


#ifdef __cplusplus
}
#endif

#endif /* RA_CALIBRATION_H */
