/*----------------------------------------------------------------------------
 * portilab.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _PORTILAB_H
#define _PORTILAB_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "poly5/tms32"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded using TMS ADC system and PortiLab")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#pragma pack(1)

struct dostime
{
	short int year;
	short int mon;
	short int day;
	short int day_of_week;
	short int hour;
	short int min;
	short int sec;
};

struct porti_header
{
	char id[31];
	short int version;
	char meas_name[81];
	short int sample_rate;
	short int storage_rate;
	unsigned char storage_type;
	short int num_sig;
	long num_sample_periods;
	long reserved1;
	struct dostime rec_start;
	long num_data_blocks;
	short int sample_periods_per_data_block;
	short int size_sample_data_per_block;
	short int compression;
	unsigned char reserved2[64];
};

struct signal_desc
{
	char name[41];
	long reserved1;
	char unit[11];
	float min_value_unit;
	float max_value_unit;
	float min_value_adc;
	float max_value_adc;
	short int index;
	short int offset;
	unsigned char reserved2[60];
};

struct data_block_hdr
{
	long index;
	long reserved1;
	struct dostime block_time;
	unsigned char reserved2[64];
};

#pragma pack()

struct channel_info
{
	struct signal_desc signal_desc;
	int bit32;
};

struct time_info
{
	/* values in sampleunits from midnight */
	unsigned long rec_start;
	unsigned long rec_end;
	unsigned long anal_start;
	unsigned long anal_end;

	char *rec_date;
	char *anal_date;
};

struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	struct porti_header hdr;

	int num_channels;
	struct channel_info *ci;

	struct time_info ti;

	struct ra_est_ch_infos *ch_type_infos;
	double mm_sec;
};


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
	RA_INFO_REC_CH_MIN_ADC_D,
	RA_INFO_REC_CH_MAX_ADC_D,
	RA_INFO_REC_CH_MIN_UNIT_D,
	RA_INFO_REC_CH_MAX_UNIT_D,
	RA_INFO_REC_CH_CENTER_VALUE_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

/* int _infos_obj[] = { }; */
/* int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]); */
int *_infos_obj = NULL;
int _num_infos_obj = 0;


int pl_check_meas(const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle mh, int minfo_type, int index, value_handle vh);
int pl_get_info_id(any_handle mh, int id, value_handle vh);
int get_info_id(any_handle mh, int id, value_handle vh);
int pl_set_info(any_handle mh, value_handle vh);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw);
int read_data_block(struct info *i, size_t num, WORD * buf, FILE * fp);

void free_info_struct(struct info *i);
void pascalstr2sz(char *string);

int get_data(struct info *i);
int get_times(struct info *i);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);


#endif /* _PORTILAB_H */
