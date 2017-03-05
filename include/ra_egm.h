/*----------------------------------------------------------------------------
 * ra_egm.h
 *
 *
 * Author(s): Raphael Schneider (raphael.schneider@medtronic.com)
 *
 * Copyright (C) 2007-2009, Raphael Schneider
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _RA_EGM_H
#define _RA_EGM_H

#include "ra_defines.h"

#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

#ifdef __cplusplus
extern "C" {
#endif


/* ---------- defines ---------- */
/* location of event */
#define EGM_LOC_UNKNOWN          0x0000	/* unknown */
#define EGM_LOC_ATRIUM           0x0001	/* atrial event */
#define EGM_LOC_VENTRICLE        0x0002	/* ventricular event */
#define EGM_LOC_LEFT_VENTRICLE   0x0004	/* left ventricular event */
#define EGM_LOC_BI_VENTRICLE     0x0008	/* bi-ventricular event */

#define EGM_TYPE_SENSED        0x0001	/* sensed event */
#define EGM_TYPE_PACED         0x0002	/* paced event */
#define EGM_TYPE_SENSED_REF    0x0004	/* sensed-refractory event */
#define EGM_TYPE_SENSED_BLANK  0x0008	/* sensed-blanking event */
#define EGM_TYPE_THERAPY_PACE  0x0010   /* Therapy pace */
#define EGM_TYPE_SENSED_VT     0x0020   /* VT sense*/
#define EGM_TYPE_SENSED_FVT    0x0040   /* FVT sense */
#define EGM_TYPE_SENSED_VF     0x0080   /* VF sense */
#define EGM_TYPE_MODE_SWITCH   0x0100   /* Mode Change/Switch event */
#define EGM_TYPE_CARDIOVERSION 0x0200   /* Cardioversion pulse */

/* macros accessing marker infos */
#define GET_LOC(x)       ((long)x & 0x0000ffff)
#define GET_TYPE(x)      (((long)x & 0xffff0000) >> 16)

#define SET_LOC(x,y)     do { y &= 0xffff0000; y |= ((long)x & 0x0000ffff); } while(0)
#define SET_TYPE(x,y)    do { y &= 0x0000ffff; y |= (((long)x << 16) & 0xffff0000); } while(0)


#define IS_UNKNOWN_LOC(x)		(GET_LOC(x) == EGM_LOC_UNKNOWN)
#define IS_ATRIUM(x)			(GET_LOC(x) == EGM_LOC_ATRIUM)
#define IS_VENTRICLE(x)			(GET_LOC(x) == EGM_LOC_VENTRICLE)
#define IS_LEFT_VENTRICLE(x)	(GET_LOC(x) == EGM_LOC_LEFT_VENTRICLE)
#define IS_BI_VENTRICLE(x)		(GET_LOC(x) == EGM_LOC_BI_VENTRICLE)

#define IS_A_SENSED(x)			(IS_ATRIUM(x) && (GET_TYPE(x) & EGM_TYPE_SENSED))
#define IS_A_PACED(x)			(IS_ATRIUM(x) && (GET_TYPE(x) & EGM_TYPE_PACED))
#define IS_A_SENSED_REF(x)		(IS_ATRIUM(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_REF))
#define IS_A_SENSED_BLANK(x)	(IS_ATRIUM(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_BLANK))
#define IS_V_SENSED(x)			(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED))
#define IS_V_PACED(x)			(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_PACED))
#define IS_V_SENSED_REF(x)		(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_REF))
#define IS_V_SENSED_BLANK(x)	(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_BLANK))
#define IS_LV_PACED(x)			(IS_LEFT_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_PACED))
#define IS_BIV_PACED(x)         (IS_BI_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_PACED))
#define IS_V_PACED_THERAPY(x)	(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_THERAPY_PACE))
#define IS_V_SENSED_VT(x)		(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_VT))
#define IS_V_SENSED_FVT(x)		(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_FVT))
#define IS_V_SENSED_VF(x)		(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_SENSED_VF))
#define IS_MODE_SWITCH(x)		(GET_TYPE(x) & EGM_TYPE_MODE_SWITCH)
#define IS_A_CARDIOVERSION(x)	(IS_ATRIUM(x) && (GET_TYPE(x) & EGM_TYPE_CARDIOVERSION))
#define IS_V_CARDIOVERSION(x)	(IS_VENTRICLE(x) && (GET_TYPE(x) & EGM_TYPE_CARDIOVERSION))


/* -------------------- globals -------------------- */
/* ---------- beat classification texts ---------- */
static const char *ra_egm_location2string[] = {
	"A",	/*  0 */
	"V",	/*  1 */
	"LV",	/*  2 */
	"BiV",	/*  3 */
	"?"		/*  4 */
}; /* ra_egm_location2string */

static const char *ra_egm_type2string[] = {
	"As",	/*  0 */
	"Ap",	/*  1 */
	"Ar",	/*  2 */
	"Ab",   /*  3 */
	"Vs",	/*  4 */
	"Vp",	/*  5 */
	"Vr",	/*  6 */
	"Vb",	/*  7 */
	"LVp",	/*  8 */
	"BiVp",	/*  9 */
	"Tp",	/* 10 */
	"Ts",	/* 11 */
	"Fs",	/* 12 */
	"Fs",	/* 13 */
	"MS",	/* 14 */
	"CD",	/* 15 */
	"??"	/* 16 */
}; /* ra_egm_type2string */


#ifdef WIN32
_inline
#else
inline
#endif
const char *
raegm_get_location_string(long c)
{
	if (IS_ATRIUM(c))
                return ra_egm_location2string[0];
	else if (IS_VENTRICLE(c))
                return ra_egm_location2string[1];
	else if (IS_LEFT_VENTRICLE(c))
                return ra_egm_location2string[2];
	else if (IS_BI_VENTRICLE(c))
                return ra_egm_location2string[3];
	else
                return ra_egm_location2string[4];
} /* raegm_get_location_string() */


#ifdef WIN32
_inline
#else
inline
#endif
const char*
raegm_get_type_string(long c)
{
	if (GET_TYPE(c) == 0)
                return raegm_get_location_string(c);
        else if (IS_A_SENSED(c))
                return ra_egm_type2string[0];
        else if (IS_A_PACED(c))
                return ra_egm_type2string[1];
        else if (IS_A_SENSED_REF(c))
                return ra_egm_type2string[2];
        else if (IS_A_SENSED_BLANK(c))
                return ra_egm_type2string[3];
        else if (IS_V_SENSED(c))
                return ra_egm_type2string[4];
        else if (IS_V_PACED(c))
                return ra_egm_type2string[5];
        else if (IS_V_SENSED_REF(c))
                return ra_egm_type2string[6];
        else if (IS_V_SENSED_BLANK(c))
                return ra_egm_type2string[7];
        else if (IS_LV_PACED(c))
                return ra_egm_type2string[8];
        else if (IS_BIV_PACED(c))
                return ra_egm_type2string[9];
        else if (IS_V_PACED_THERAPY(c))
                return ra_egm_type2string[10];
        else if (IS_V_SENSED_VT(c))
                return ra_egm_type2string[11];
        else if (IS_V_SENSED_FVT(c))
                return ra_egm_type2string[12];
        else if (IS_V_SENSED_VF(c))
                return ra_egm_type2string[13];
        else if (IS_MODE_SWITCH(c))
                return ra_egm_type2string[14];
        else if (IS_A_CARDIOVERSION(c) || IS_V_CARDIOVERSION(c))
                return ra_egm_type2string[15];
        else
                return ra_egm_type2string[16];
} /* raegm_get_class_string() */

#ifdef __cplusplus
}
#endif

#endif /* _RA_EGM_H */
