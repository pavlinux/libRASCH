/*----------------------------------------------------------------------------
 * mortara_sparc.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2005, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef MORTARA_SPARC_H
#define MORTARA_SPARC_H

#define PLUGIN_VERSION  "0.1.0"
#define PLUGIN_NAME     "mortara-sparc"
#define PLUGIN_DESC     gettext_noop("Handle ECGs stored in the (Mortara) SPARC format")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL

#define NUM_CH_REC   8
#define NUM_CH      12

#define PAT_INFO_LEN  40

#define MAX_NAME  8
#define MAX_UNIT  8

#pragma pack(1)

struct hdr_struct
{
	unsigned short mode;
	char undef1[2670];

	short forename[20];
	short name[20];
	short id[20];
	short dob[20];
	short gender[20];
	short history[20];
	short comment[20];

	short undef2;

	unsigned short qrs_onset;
	unsigned short qrs_offset;
	unsigned short qrs_onset2;
	unsigned short qrs_offset2;

	short undef3;

	unsigned long num_qrs_detect;
	unsigned long num_qrs_average;

	unsigned long samples_average[5000][9];

	char rec_date[8];
	char undef4[2];
	char rec_time[5];
	char undef5[17];

	unsigned long start_sample;
	unsigned short ref_time;

	unsigned short num_valid_rri;
	unsigned short rri[600];
	char rri_annot[600];

	unsigned short btf_25;
	unsigned long btf_noise;
	unsigned short btf_end_late_pot;
	unsigned long btf_rms;
	unsigned short btf_duration;
	unsigned short btf_samples[400][9];
	unsigned short btf_filter_data[3607];
	
	unsigned long eigen_dep[8];
	unsigned long eigen_rep[8];
}; /* struct hdr_struct */

#pragma pack()

struct timestamp
{
	short int year;
	short int mon;
	short int day;
	short int hour;
	short int min;
	short int sec;
	short int msec;
}; /* struct timestamp */


struct pat_info
{
	char name[PAT_INFO_LEN];
	char forename[PAT_INFO_LEN];
	char id[PAT_INFO_LEN];
	char dob[PAT_INFO_LEN];
	char gender[PAT_INFO_LEN];
	char history[PAT_INFO_LEN];
	char comment[PAT_INFO_LEN];
}; /* struct pat_info */


struct ch_info
{
	char name[MAX_NAME];
	char unit[MAX_UNIT];
}; /* struct ch_info */


struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	double samplerate;

	struct hdr_struct hdr;

	struct pat_info pi;
	struct timestamp rec_start;

	struct ch_info ci[NUM_CH];
	struct ra_est_ch_infos ch_type_infos[NUM_CH];
	double mm_sec;

	long num_samples;
	unsigned short *samples_orig;
	unsigned short *samples;
	double *samples_scaled;
	double sample_mean[NUM_CH];
}; /* struct info */


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
	RA_INFO_REC_GEN_MM_SEC_D,
};
int _num_infos_rec_gen = sizeof(_infos_rec_gen) / sizeof(_infos_rec_gen[0]);

int _infos_rec_ch[] = {
	RA_INFO_REC_CH_NAME_C,
	RA_INFO_REC_CH_DESC_C,
	RA_INFO_REC_CH_NUM_SAMPLES_L,
	RA_INFO_REC_CH_SAMPLERATE_D,
	RA_INFO_REC_CH_NUM_BITS_L,
	RA_INFO_REC_CH_AMP_RESOLUTION_D,
	RA_INFO_REC_CH_SAMPLE_TYPE_L,
	RA_INFO_REC_CH_UNIT_C,
	RA_INFO_REC_CH_MIN_UNIT_D,
	RA_INFO_REC_CH_MAX_UNIT_D,
	RA_INFO_REC_CH_CENTER_VALUE_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int _infos_obj[] = {
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PATIENT_ID_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
	RA_INFO_OBJ_PATIENT_REASON_C,
	RA_INFO_OBJ_PERSON_COMMENT_C,
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);

int pl_check_meas(const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int pl_get_info_id(any_handle h, int id, value_handle vh);
int get_info_id(any_handle h, int id, value_handle vh);
int pl_set_info(any_handle h, value_handle vh);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw);

void free_info_struct(struct info *i);

int get_data(struct info *i, int fast);
int get_pat_info(struct info *i);
int extract_string(const char *src, char *dest);
int get_times(struct info *i);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

#endif /* MORTARA_SPARC_H */
