/*----------------------------------------------------------------------------
 * ctg.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef CTG_H
#define CTG_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "ctg-rasch"
#define PLUGIN_DESC     gettext_noop("Handle cardiotocogram signals exported from GMT system")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL



struct pat_info
{
	char *name;
	char *forename;
	char *birthday;
	char *gender;

	char *gestation_date;
	int num_fetus;
}; /* struct pat_info */

struct rec_info
{
	int hour;
	int min;
	int sec;
	int day;
	int month;
	int year;		/* complete, 4 digits */
}; /* struct rec_info */


struct ch_info
{
	char name[5];
	char desc[30];
	double samplerate;
	long num_samples;

	char unit[5];
	double min_value;
	double max_value;
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

	struct pat_info pi;
	struct rec_info ri;

	int num_channels;
	struct ch_info ci[3];

	double mm_sec;
	struct ra_est_ch_infos *ch_type_infos;
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
	RA_INFO_REC_CH_DO_IGNORE_VALUE_L,
	RA_INFO_REC_CH_IGNORE_VALUE_D,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int _infos_obj[] = {
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
	RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L,
	RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C,
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);


int pl_check_meas(const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle mh, int minfo_type, int index, value_handle vh);
int pl_get_info_id(any_handle mh, int id, value_handle vh);
int get_info_id(any_handle mh, int id, value_handle vh);

int set_utf8(value_handle vh, const char *string);

int pl_set_info(any_handle mh, int id, value_handle vh);
int write_single_info(struct info *i, char **store,  value_handle data);
int parse_time_or_date(const char *string, int *part1, int *part2, int *part3);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, BYTE *buf_f);

int check_file(struct info *i, const char *file);
void free_info_struct(struct info *i);

int get_data(struct info *i);
int read_patinfo(struct info *i);
void split_timestamp(struct info *i, char *string);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

#endif /* CTG_H */
