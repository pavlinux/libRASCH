/*----------------------------------------------------------------------------
 * ra_respiration.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_RESPIRATION_H
#define RA_RESPIRATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   TODO: think about what this plugin should really
         do and how this can be implemented
	 (up to now it is related to ecgs, should
	 it be more general or not; if not, rename
	 it to show ecg-relation)

!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
struct ra_respiration
{
	short use_eval;  /* use values from evaluation */
	short use_start_end_pos;  /* use values between start_pos and end_pos */
	long start_pos; /* start-pos in sample-units */
	long end_pos;  /* end-pos in sample-units */

	rec_handle rh;  /* recording handle */
	long num_ch;  /* number of channels set in 'ch' */
	long *ch;  /* list of respiration channels */

	short save_in_eval;  /* flag if results should be saved in an evaluation */

	eval_handle eh;  /* eval-handle used to store the results */
	long num_clh;  /* number of event-class's set in 'clh' */
	class_handle *clh;  /* event-class-handle(s) used to store the phase information */
}; /* struct ra_respiration */


#ifdef __cplusplus
}
#endif


#endif /* RA_RESPIRATION_H */
