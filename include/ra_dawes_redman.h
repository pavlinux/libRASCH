/*----------------------------------------------------------------------------
 * ra_dawes_redman.h
 *
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef RA_DAWES_REDMAN_H
#define RA_DAWES_REDMAN_H

#pragma pack(1)

struct ra_dawes_redman
{
	eval_handle eh;

	short use_start_end_pos;
	long start_pos;
	long end_pos;

	rec_handle rh;
	long ch_num;
	short use_ignore_value;
	double ignore_value;
	short filter_maternal_pulse;

	/* infos if some regions and/or events should be ignored */
	short ignore_marked_regions;  /* ignore regions which are marked in the annotations with the IGNORE flag */
	short ignore_noise_regions;   /* ignore regions which are marked in the annotations with the NOISE flag */
}; /* struct ra_dawes_redman */

#pragma pack()

#endif /* RA_DAWES_REDMAN_H */
