/*----------------------------------------------------------------------------
 * ra_fiducial_point.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_FIDUCIAL_POINT_H
#define RA_FIDUCIAL_POINT_H

struct ra_fiducial_point
{
	short use_class;     /* align events stored in option 'clh' */
	class_handle clh;   /* handle to the event-class holding the events which should be processed */
	char *pos_prop;     /* ASCII-id of the event-property holding the positions (optional) */
	short save_in_class; /* flag if the fiducial-points should be saved in the event-class */

	long num_data;      /* number of event-id's/sample-positions */
	long *data;         /* event-id's/sample-positions */
	short data_is_pos;   /* flag if values in 'data' are event-id's (=0) or sample positions (=1) */

	long ch;            /* channel where fiducial point should be searched */
	double win_len;     /* area (+-'win_len') where the fiducial-point will be searched [sec] */
	double corr_len;    /* length of correlation [sec] */
}; /* struct ra_fiducial_point */


#endif /* RA_FIDUCIAL_POINT_H */
