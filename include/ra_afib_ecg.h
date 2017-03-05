/*----------------------------------------------------------------------------
 * ra_afib_ecg.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2004-2006, Raphael Schneider
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_AFIB_ECG_H
#define RA_AFIB_ECG_H

#pragma pack(1)

struct ra_afib_ecg
{
	eval_handle eh;	/* evaluation used; if '= NULL' use default evaluation */

	/* selection of input-data */
	short use_class; /* use data from the event-class given in 'clh' */
	class_handle clh; /* event class with the data used for the calculations */

	/* selection of input-data */
	/* use events which are between start-pos-sample and end-pos-sample */
	short use_start_end_pos;
	long start_pos;
	long end_pos;
	
	short dont_use_vent_rri; /*  flag if VPC related RR intervals should be removed */
	short use_complete_only; /* use only segments when all values are valid */

	short save_in_eval;	/* save results in eval-file (not supported yet) */
}; /* struct ra_afib_ecg */

#pragma pack()

#endif /* RA_AFIB_ECG_H */
