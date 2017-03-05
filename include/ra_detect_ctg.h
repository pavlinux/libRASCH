/**
 * \file ra_detect_ctg.h
 *
 * header file for 'detect-ctg' process plugin
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id: $
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_DETECT_CTG_H
#define RA_DETECT_CTG_H

struct ra_detect_ctg
{
	long num_ch; /* number of channels set in 'ch' */
	long *ch; /* list of channels used for the event detection */

	short save_in_eval; /* flag if results should be saved in an evaluation */
	eval_handle eh; /* eval-handle used to store the results */
	class_handle clh; /* event-class-handle used to store the results */
}; /* struct ra_detect_ctg */

#endif  /* RA_DETECT_CTG_H */
