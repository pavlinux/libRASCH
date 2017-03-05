/**
 * \file ra_estimate_ch_infos.h
 *
 * The header file contains informations to estimate the type
 * of a recording channel. 
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_ESTIMATE_CH_INFOS_H
#define RA_ESTIMATE_CH_INFOS_H

#include <ra.h>

#ifdef __cplusplus
extern "C" {
#endif


struct ch_map
{
	char *ch_name;
	long ch_type;
	int inf_idx;
}; /* struct ch_map */


/**
 * \struct ra_est_ch_infos
 * \brief contains informations about a channel type
 * \param <name_contains> the string is used in the regexp /^.*[^a-zA-Z]string[^a-zA-Z]/i to estimate the channel type
 * \param <type> type of the channel (see RA_CH_TYPE_* in ra_defines.h)
 * \param <parent_type> if the type is a sub-type, this is the main-type which has to be also "mentioned" in the ch-name
 * \param <has_sub_types> flag if sub-types are possible
 * \param <mm_sec> mm per second (hint for displaying signal)
 * \param <mm_unit> mm per unit (hint for displaying signal)
 * \param <offset> vertical offset (value will be subtracted from raw-data shown)
 * \param <centered> flag if raw-data should be shown centered
 * \param <center_value> value which represents center value
 * \param <min_max> flag if min/max values should be used for lower/upper channel border
 * \param <min> value used for lower channel border
 * \param <max> value used for upper channel border
 */
struct ra_est_ch_infos
{
	char name_contains[50];	/* /^.*[^a-zA-Z]name_contains[^a-zA-Z]/i */
	long type;
	long parent_type;
	int has_sub_types;
	double mm_sec;		/* mm per second (hint for displaing signal) */
	double mm_unit;		/* mm per unit (hint for displaing signal) */
	double offset;		/* vertical offset (value will be substracted
				   from value shown */
	int centered;
	double center_value;

	int min_max;
	double min;
	double max;
}; /* struct ra_est_ch_infos */


static struct ra_est_ch_infos _inf[] = {
	{"ecg", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"ekg", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V1", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V2", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V3", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V4", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V5", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"V6", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"MLII", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"ML5", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"CM5", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"DM5", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"aVR", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"aVL", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"aVF", RA_CH_TYPE_ECG, -1, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},

	{"egm", RA_CH_TYPE_EGM, -1, 1, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"ra", RA_CH_TYPE_EGM_RA, RA_CH_TYPE_EGM, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"la", RA_CH_TYPE_EGM_LA, RA_CH_TYPE_EGM, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"rv", RA_CH_TYPE_EGM_RV, RA_CH_TYPE_EGM, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},
	{"lv", RA_CH_TYPE_EGM_LV, RA_CH_TYPE_EGM, 0, 50.0, 10.0, 0.0, 1, 0.0, 0, 0.0, 0.0},

	{"rr", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"bloodpressure", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"bp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"abp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"blutdruck", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	/*{"pleth", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 0.0, 0, 0.0, 0, 0.0, 0.0},*/
	{"art", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 0.0, 0, 0.0, 0, 0.0, 0.0},
	{"pap", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 0.0, 0, 0.0, 0, 0.0, 0.0},
	{"cvp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 0.0, 0, 0.0, 0, 0.0, 0.0},
	{"cbp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"rbp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"pbp", RA_CH_TYPE_RR, -1, 0, 25.0, 0.2, 50.0, 0, 0.0, 0, 0.0, 0.0},

	{"resp", RA_CH_TYPE_RESP, -1, 0, 5.0, 1, 0, 1, 0.0, 0, 0.0, 0.0},
	{"breath", RA_CH_TYPE_RESP, -1, 0, 5.0, 1, 0, 1, 0.0, 0, 0.0, 0.0},
	{"atmung", RA_CH_TYPE_RESP, -1, 0, 5.0, 1, 0, 1, 0.0, 0, 0.0, 0.0},
	{"Atemguertel", RA_CH_TYPE_RESP, -1, 0, 5.0, 1, 0, 1, 0.0, 0, 0.0, 0.0},

	{"temperature", RA_CH_TYPE_TEMP, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},
	{"temperatur", RA_CH_TYPE_TEMP, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},
	{"temp", RA_CH_TYPE_TEMP, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},

	{"activity", RA_CH_TYPE_ACTIVITY, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},
	{"aktivität", RA_CH_TYPE_ACTIVITY, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},
	{"aktivitaet", RA_CH_TYPE_ACTIVITY, -1, 0, -1, -1, 0, 0, 0.0, 0, 0.0, 0.0},

	{"eeg", RA_CH_TYPE_EEG, -1, 0, 25.0, 100.0, 0.0, 1, 0.0, 0, 0.0, 0.0},

	{"emg", RA_CH_TYPE_EMG, -1, 0, 100.0, 100.0, 0.0, 1, 50.0, 0, 0.0, 0.0},

	{"fhr", RA_CH_TYPE_CTG_FHR, -1, 0, 0.16667, 0.5, 50.0, 0, 0.0, 0, 0.0, 0.0},
	{"uc", RA_CH_TYPE_CTG_UC, -1, 0, 0.16667, 0.4, 0.0, 0, 0.0, 0, 0.0, 0.0},

	{"angle", RA_CH_TYPE_ANGLE, -1, 0, 50.0, 0.0, 0.0, 0, 0.0, 1, 0, 360.0},
}; /* _inf[] */
int _num_infs = (int) (sizeof(_inf) / sizeof(_inf[0]));

/* TODO: think if this protoype should be moved to ra.h */
LIBRAAPI int ra_est_ch_type(ra_handle ra, const char *folder, const char *filename, int is_dir_based,
			    const char *name, struct ra_est_ch_infos *inf);

#ifdef __cplusplus
}
#endif

#endif /* RA_ESTIMATE_CH_INFOS_H */
