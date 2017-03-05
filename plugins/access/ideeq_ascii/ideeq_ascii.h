/*----------------------------------------------------------------------------
 * ideeq_ascii.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef IDEEQ_ASCII_H
#define IDEEQ_ASCII_H

#define PLUGIN_VERSION  "0.1.1"
#define PLUGIN_NAME     "ideeq-ascii"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded with IDEEQ and saved as ASCII files")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL

#define MAX_NAME   200
#define MAX_UNIT   200


#define TIME_FORMAT_NONE            0
#define TIME_FORMAT_START_OF_BLOCK  1

#define RAW_BUF_ADD   5000

struct event_entry
{
	long start_su;
	long end_su;
	char text[1024];
};  /* struct event_entry */


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


struct ch_info
{
	char name[MAX_NAME];
	char unit[MAX_UNIT];

	double bit_res;
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
	long start_sample_su;

	int num_event_entries;
	struct event_entry *events;

	int time_format;
	struct timestamp ts_rec_start;
		
	long num_ch;
	struct ch_info *ci;

	struct ra_est_ch_infos *ch_type_infos;
	double mm_sec;

	long raw_buf_size;
	long num_samples;
	double **samples;
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
	RA_INFO_REC_CH_CENTER_SAMPLE_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

/* int _infos_obj[] = { */
/* }; */
/* int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]); */
int * _infos_obj = NULL;
int _num_infos_obj = 0;

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
double get_samplerate(FILE *fp, int dec_point_is_comma);
long get_start_su(FILE *fp, int dec_point_is_comma, double samplerate);
double ascii_to_double(char *buf, int dec_point_is_comma);
void calc_rec_start_ts(struct info *i);
int read_event_list(struct info *i, FILE *fp, int dec_point_is_comma);
int get_channel_info(struct info *i, char *buf);

int parse_timestamp(struct timestamp *ts, char *string);
int get_data_line(char *buf, double *data, int n_ch, int dec_point_is_comma);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

int pl_get_eval_info(meas_handle mh, value_handle name, value_handle desc, value_handle add_ts,
					 value_handle modify_ts, value_handle user, value_handle host, value_handle prog);
long pl_get_class_num(meas_handle mh);
int pl_get_class_info(meas_handle mh, unsigned long class_num, value_handle id, value_handle name, value_handle desc);
long pl_get_prop_num(class_handle clh, unsigned long class_num);
int pl_get_prop_info(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle id,
					 value_handle type, value_handle len,
					 value_handle name, value_handle desc, value_handle unit,
					 value_handle use_minmax, value_handle min, value_handle max,
					 value_handle has_ign_value, value_handle ign_value);
int pl_get_ev_info(class_handle clh, unsigned long class_num, value_handle start, value_handle end);
int pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num,
					long event, value_handle value, value_handle ch);
int pl_get_ev_value_all(prop_handle ph, unsigned long class_num, unsigned long prop_num,
						value_handle value, value_handle ch);
int get_all_annot_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);

#endif /* IDEEQ_ASCII_H */
