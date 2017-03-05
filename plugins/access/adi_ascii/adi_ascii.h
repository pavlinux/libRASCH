/*----------------------------------------------------------------------------
 * adi_ascii.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2005-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef ADI_ASCII_H
#define ADI_ASCII_H

#define PLUGIN_VERSION  "0.1.1"
#define PLUGIN_NAME     "adi-ascii"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded with Powerlab and saved as ASCII files")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL

#define MAX_NAME   200
#define MAX_UNIT   200


#define TIME_FORMAT_NONE            0
#define TIME_FORMAT_START_OF_BLOCK  1

#define RAW_BUF_ADD   5000


struct elements
{
	int num;
	char data[64][200];
	double value[64];
}; /* struct elements */


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
	
	double range;
	double bit_res;

	double top_value;
	double bottom_value;
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
int get_data_elem(char *buf, struct elements *elem, int dec_point_is_comma);
int prep_ch_info(struct info *i, int n);
int parse_timestamp(struct timestamp *ts, char *string);
int get_data_line(char *buf, double *data, int n_ch, int dec_point_is_comma);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

#endif /* ADI_ASCII_H */
