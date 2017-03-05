/*----------------------------------------------------------------------------
 * read_rri.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2003-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef READ_RRI_H
#define READ_RRI_H

#define PLUGIN_VERSION  "0.4.1"
#define PLUGIN_NAME     "read-rri"
#define PLUGIN_DESC     gettext_noop("Handle RR interval-files (nearly the final design)")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#pragma pack(1)

struct reynolds_hdr
{
	char header[64];
	char version[8];
	int start_time_min;
	int end_time_min;
	int first_interval_ms;
	unsigned short previous_beat_type;
	char not_used[64];
}; /* struct reynolds_hdr*/

/* read structures for Altair System */
#include "occ.h"

/* structures for QRSDK format */

struct qrsdk_header
{
    char pat_name[40];		/* ascii patient name */
    char pat_id[16];		/* ascii patient id */
    char extension[4];		/* file extension (on 8000) */
    char rec_time[10];		/* tape start time HH:MM:SS */
    char rec_date[10];		/* tape start date DD-MMM-YY */
    char save_date[10];		/* file date (on 8000) DD-MMM-YY */
    short samplerate;		/* samples/sec (usually 120-128) */
    short max_rri;		/* maximum rr interval limit (in sample times) */
    short reserved[207];
    short software_version;	/* software version N.M (100*N + M) */
    short checksum;		/* checksum such that 0=sum(256 words) */
};  /* struct qrsdk_header */

struct qrsdk_beat
{
    unsigned char clock_inc;	/* clock increment in sample times */
    unsigned char label;	/* event label */
};  /* struct qrsdk_beat */


#pragma pack()


struct beat_info
{
	long pos;
	long annotation;
};  /* struct beat_info */

#define MAX_FIELD_LEN   80

/* Reynolds Beat Types (other values are set to artifact) */
#define NULL_BEAT        0
#define NORMAL_BEAT      2
#define ABERRANT_BEAT    3
#define PACED_BEAT       6



struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	double samplerate;

	int pos_is_rri;
	int rri_in_msec;

	char id[MAX_FIELD_LEN];
	char lastname[MAX_FIELD_LEN];
	char forename[MAX_FIELD_LEN];
	char birthday[MAX_FIELD_LEN];
	char rec_date[MAX_FIELD_LEN];
	char rec_time[MAX_FIELD_LEN];
	char save_date[MAX_FIELD_LEN];

	int num_files;
	char **files;
	long size;

	long num_beats;
	struct beat_info *beats;
};  /* struct info */


char *_supported_extensions[] = {
	"rri", "qrs", "dat", "mis", "mib", "mif", "occ", "txt"
};
int _num_supported_extensions = sizeof(_supported_extensions) / sizeof(_supported_extensions[0]);

int _infos_rec_gen[] = {
	RA_INFO_REC_GEN_TYPE_L,
	RA_INFO_REC_GEN_NAME_C,
	RA_INFO_REC_GEN_DESC_C,
	RA_INFO_REC_GEN_TIME_C,
	RA_INFO_REC_GEN_DATE_C,
	RA_INFO_REC_GEN_DURATION_SEC_D,
	RA_INFO_REC_GEN_NUMBER_L,
	RA_INFO_REC_GEN_NUM_SUB_REC_L,
	RA_INFO_REC_GEN_NUM_CHANNEL_L,
	RA_INFO_REC_GEN_OFFSET_L,
	RA_INFO_REC_GEN_PATH_C,
	RA_INFO_REC_GEN_DIR_L,
	RA_INFO_REC_CH_SAMPLERATE_D,
};
int _num_infos_rec_gen = sizeof(_infos_rec_gen) / sizeof(_infos_rec_gen[0]);

int *_infos_rec_ch = NULL;
/*int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]); */
int _num_infos_rec_ch = 0;

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int _infos_obj[] = {
	RA_INFO_OBJ_PATIENT_ID_C,
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);


int pl_check_meas(const char *name);
int get_extension(struct info *i);

int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int pl_get_info_id(any_handle h, int id, value_handle vh);
int get_info_id(any_handle h, int id, value_handle vh);
int pl_set_info(any_handle h, value_handle vh);

void free_info_struct(struct info *i);

int get_data(struct info *i, int fast);
int check_if_qrsdk(FILE *fp);
int check_if_reynolds_old_rrtools(FILE *fp);
int check_if_getemed_rri(FILE *fp);

int read_librasch_rri(struct info *i, FILE *fp, char *version, int fast);
int read_reynolds_rri(struct info *i, FILE *fp, int fast);
int read_reynolds_rri_old(struct info *i, FILE *fp, int fast);
int read_marquette_rri(struct info *i, FILE *fp, int fast);
int read_altair_occ(struct info *i, FILE *fp);
int read_qrsdk(struct info *i, FILE *fp, int fast);
int read_getemed_rri(struct info *i, FILE *fp, int fast);
int read_other_formats(struct info *i, FILE *fp, int qrs_file);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

int pl_get_eval_info(meas_handle mh, value_handle name, value_handle desc, value_handle add_ts,
		     value_handle modify_ts, value_handle user, value_handle host, value_handle prog);
long pl_get_class_num(meas_handle mh);
int pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc);
long pl_get_prop_num(class_handle clh, unsigned long class_num);
int pl_get_prop_info(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle id, value_handle type, value_handle len,
		     value_handle name, value_handle desc, value_handle unit,
		     value_handle use_minmax, value_handle min, value_handle max,
		     value_handle has_ign_value, value_handle ign_value);
int pl_get_ev_info(class_handle clh, unsigned long class_num, value_handle start, value_handle end);
int pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch);

int pl_get_ev_value_all(prop_handle ph, unsigned long class_num, unsigned long prop_num, value_handle value, value_handle ch);
int get_all_beat_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);


#endif /* READ_RRI_H */
