/*----------------------------------------------------------------------------
 * ra_detect.h
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2005-2008, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_DETECT_H
#define RA_DETECT_H

/* flags for ecg morphology */
#define ECG_P_WAVE_OK    0x00000001
#define ECG_QRS_OK       0x00000100
#define ECG_T_WAVE_OK    0x00010000
#define ECG_U_WAVE_OK    0x01000000

/* constants for T-wave end algorithm */
#define T_WAVE_END_ALGO_DEFAULT  0
#define T_WAVE_END_ALGO_TANGENT  1
#define T_WAVE_END_ALGO_AREA     2
#define T_WAVE_END_ALGO_LINE     3

/* constants which 'wave'-limits should be checked */
#define CHECK_P    0
#define CHECK_QRS  1
#define CHECK_T    2


struct ra_detect
{
	long num_ch;               /* number of channels set in 'ch' */
	long *ch;                  /* list of channels used for the event detection */
	short combine_beats;        /* flag if nearby beats (distance below 'min_beat_distance') should be combined */
	double min_beat_distance;  /* minimum allowed distance between beats [seconds] */
	short save_in_eval;         /* flag if results should be saved in an evaluation */
	eval_handle eh;            /* eval-handle used to store the results */
	class_handle clh;	/* event-class-handle used to store the results when region was selected */

	short use_region;	/* search beats in a given area */
	long region_start;	/* start position of the search area */
	long region_end;	/* end position of the search area */
	short region_start_is_beatpos;  /* 'region_start' is the beat position */

	/* heartbeat-events for which the wave-boundary detection should be re-run
	   (most of the time with some constraints given below (e.g. forced morphology type))*/
	long num_events;
	long *events;

 	short filter_powerline_noise; /* flag if a power-line noise filter should be applied */
 	short filter_baseline_wander; /* flag if a baseline-wander filter should be applied */

	short check_for_calibration; /* flag if for a calibration signal should be searched */

 	short do_interpolation;	/* flag if no interpolation should be performed for signals
				   with lower samplerate (below 500Hz) */
	
 	long t_wave_algorithm;	/* selects which T-wave-end algortithm should be used:
				   0: default (line method)
				   1: tangent method
				   2: area method
				   3: line method
				*/

	/* wave-form morphology types which should be the P-/QRS-/T-Wave
	   forced to (used when editing an ecg) */
	long force_p_type;
	long force_qrs_type;
	long force_t_type;

	/* options when the wave limits should be derived around a specific position */
	short check_wave_at_cursor;
	long edit_cursor_pos; 
	long wave_to_check;  /* selects which 'wave'-limits should be checked:
				0: P-wave
				1: QRS-complex
				2: T-wave
			     */


	/* percentage of threshold levels used in wave detection (1=100%) */
	double thresh_p;
	double thresh_qrs;
	double thresh_t;
};  /* struct ra_detect */


#endif  /* RA_DETECT_H */
