/*----------------------------------------------------------------------------
 * occ.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id:  $
 *--------------------------------------------------------------------------*/

#ifndef OCC_H
#define OCC_H


#define OCC_CH1_EVENT  0x01
#define OCC_CH2_EVENT  0x02
#define OCC_CH3_EVENT  0x04

/* The beat record file *.occ contains beat related information.
   Each beat record is 16 bytes in size and is contained in this file in chronological order. */

struct altair_pca_ann /* defines the 3-channel Altair/PC annotation format */
{              /* number of bytes in first section */
	unsigned char  label;          /* 1 - beat classification label, see enum BEAT_TYPE below */
	unsigned char index;           /* 1 - index into cluster or form table, 0xFF = not clustered ( unsigned char) */
 	WORD rr;                       /* 2 - coupling interval (in sp), 0xFFFF = gap indicator ( unsigned short) */
	unsigned long  atime;          /* 4 - time of annotation in sample points 5 msec resolution */

	unsigned char  hr;            /* 1 - local heart rate: 0 - 250 bpm, 0xFF = no heart rate  */
	unsigned char misc;           /* 1 - miscellaneous byte to keep structure even  */

 	/* 1 - channel status bit fields  */
 	/* got an event flag  */
	/* because GCC prints warnings about bit-fields, use ch_f */
	unsigned char ch_f;
/* 	unsigned char ch1_f : 1;        /\* CH1 event flag  *\/ */
/* 	unsigned char ch2_f : 1;        /\* CH2 event flag  *\/ */
/* 	unsigned char ch3_f : 1;        /\* CH3 event flag  *\/ */

/* 	unsigned char unused1_bits:5; /\* keep byte aligned  *\/ */


 	/* 1 - channel corruption indicators */
 	/* encoded in two bits: */
 	/*  0 = no corruption, */
 	/*  1 = wander or drift, */
	/*  2 = moderate artifact */
 	/*  3 = severe artifact or rail  */
	/* because GCC prints warnings about bit-fields, use ch_corrupt_ind */
	unsigned char ch_corrupt_ind;
/* 	unsigned char ch1_cor:2;      /\* CH1 corruption indicator  *\/ */
/* 	unsigned char ch2_cor:2;      /\* CH2 corruption indicator  *\/ */
/* 	unsigned char ch3_cor:2;      /\* CH3 corruption indicator  *\/ */

/* 	unsigned char beat_conf:2;    /\* beat classification confidence: 0 = lowest -> 4 = highest  *\/ */

	/* 1 - ST slope bit fields */
	/* because GCC prints warnings about bit-fields, use ch_st_status */
	unsigned char ch_st_status;
/* 	unsigned char ch1_sts:2;      /\* CH1 ST slope: 0 = down, 1 = flat, 2 = up, 3 = NO STS  *\/ */
/* 	unsigned char ch2_sts:2;      /\* CH2 ST slope: 0 = down, 1 = flat, 2 = up, 3 = NO STS  *\/ */
/* 	unsigned char ch3_sts:2;      /\* CH3 ST slope: 0 = down, 1 = flat, 2 = up, 3 = NO STS  *\/ */

/* 	unsigned char unused2_bits:2; /\* keep byte aligned  *\/ */

	char stl[3];                  /* 3 - ST level for 3 channels in tenth-mm, -50 -> +50, 0x7F = NO ST info.  */
                                 
};  /* struct altair_pca_ann (16 bytes total) */


enum ALTAIR_BEAT_TYPE
{
	UNUSED = 0x00, /* invalid beat type */
	/* all beat annotation codes for beats considered normal and Supraventricular  */
	FIRST_NRM_CODE = 0x1D, /* first entry code for 'normal' beats  */
	LAST_NRM_CODE = 0x25,      /* last entry code for 'normal' beats  */

	FIRST_SVE_CODE = 0x30,     /* first entry code for 'Supraventricular' beats  */
	LAST_SVE_CODE = 0x33,      /* last entry code for 'Supraventricular' beats  */

	/* all beat annotation codes for beats considered abnormal (ventricular):  */
	FIRST_ABN_CODE = 0x61,     /* first entry code for 'abnormal' beats  */
	LAST_ABN_CODE = 0x66,      /* last entry code for 'abnormal' beats  */

	LAST_BEAT_CODE = 0x66,     /* last entry code for 'true' beats  */

	/* all annotation codes above LAST_BEAT_CODE represent non-beat entries:  */
	NOISE        = 0xF0,       /* noise code  */
	PWAVE        = 0xF1,       /* p_wave event  */
	TWAVE        = 0xF2,       /* t_wave event  */
	CAL_ALT      = 0xF3,       /* calibration pulse  */
	ALG_SUSP     = 0xF4,       /* algorithm suspension due to corruption  */
	UNDEFINED    = 0xF9,       /* undefined beat, not yet classified  */
	DELETE_ALT   = 0xFC,       /* deleted beat  */
	TIME_DELETE  = 0xFD        /* deleted time segment  */
}; 

#endif  /* OCC_H */
