/*----------------------------------------------------------------------------
 * dasylab.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef DASYLAB_H
#define DASYLAB_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "dasylab"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded using DasyLab")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#ifndef WORD
#define WORD unsigned short
#endif /* WORD */
#ifndef DWORD
#define DWORD unsigned long
#endif /* DWORD */


#define DASY_CH_TYPE_NORMAL                 0
#define DASY_CH_TYPE_BINARY                 1	/* TTL coded binary data */
#define DASY_CH_TYPE_REAL_SPEKTRUM         10
#define DASY_CH_TYPE_REAL_SPEKTRUM_HALF    11
#define DASY_CH_TYPE_REAL_SPEKTRUM_HALF_1  12
#define DASY_CH_TYPE_SPEKTRUM_SYM_X        13
#define DASY_CH_TYPE_HISTOGRAM             20
#define DASY_CH_TYPE_HISTOGRAM_W_TIME      21
#define DASY_CH_TYPE_DIG_WORD              30
#define DASY_CH_TYPE_COUNT                 60
#define DASY_CH_TYPE_UCOUNT                61
#define DASY_CH_TYPE_THERMO                70

#define DASY_CH_FLAG_CAN_HAVE_HOLES        0x0001
#define DASY_CH_FLAG_CH_HAVE_SHORT_BLOCKS  0x0002

#define MAX_INFO_SIZE  200


struct info_struct
{
	char rec_system[MAX_INFO_SIZE];
	char worksheet[MAX_INFO_SIZE];
	char author[MAX_INFO_SIZE];
	char company[MAX_INFO_SIZE];
	char division[MAX_INFO_SIZE];
	char title[MAX_INFO_SIZE];
	char data_file[MAX_INFO_SIZE];
};				/* struct info_struct */


struct global_header
{
	struct info_struct infos;
	char cpu_id[2];		/* up to now "IN" */
	WORD hdr_size;
	WORD file_format;
	WORD file_version;
	WORD hdr2_size;
	WORD ch_hdr_size;
	WORD datablock_hdr_size;
	WORD use_extra_data_file;
	WORD num_channels;
	double samplerate;
	DWORD start_time;
};				/* struct global_header */


#pragma pack(1)

/* file-format 1: Universal-Format 1 (samples are float) */
struct global_header_f1
{
	WORD hdr_size;
	WORD num_channel;
	WORD data_multiplexed;
	WORD inputch2recch[16];	/* for every input-channel recorded channel */
};				/* struct global_header_f1 */


struct ch_header_f1
{
	WORD hdr_size;
	WORD ch_number;
	WORD block_size;
	double samplerate;
	WORD ch_type;
	WORD flags;
	double not_used1;
	double not_used2;
	char unit[MAX_INFO_SIZE];
	char name[MAX_INFO_SIZE];
};				/* struct ch_header_f1 */


struct data_header_f1
{
	char id[4];		/* "DATA" */
	WORD hdr_size;
	long starttime;
	DWORD not_used[2];
};				/* struct data_header_f1 */


struct datablock_header_f1
{
	WORD ch;
	double block_start;
	double samplerate;
	WORD num_samples;
};				/* struct datablock_header_f1 */


/* file-format 0: streamer-format (samples are WORD directly from hardware-driver) */
struct global_header_f0
{
	WORD hdr_size;
	WORD num_analog_ch;
	WORD num_counter_ch;
	WORD num_digital_ports;
	WORD num_bits_digital_port;
	DWORD samplenumber_oldest_saved_sample;
	DWORD number_samples_per_channel;
};				/* struct global_header_f0 */


struct ch_header_f0
{
	WORD hdr_size;
	WORD ch_type;
	char ch_names[384];	/* 16*24 */
	WORD data_format;
	double analog_scale;
	double analog_offset;
};				/* struct ch_header_f0 */


/* file-format 2: Universal-Format 2 (samples are double)
   this format seems not to be used (TODO: add version of user-manual and date where this is told on page 5-13) */
struct global_header_f2
{
	WORD hdr_size;
};				/* struct global_header_f2 */


struct ch_header_f2
{
	WORD hdr_size;
	WORD block_size;
	double samplerate;
	WORD ch_type;
	short data_type;	/* 0:equidistant  1:triggerd */
	double analog_scale;
};				/* struct ch_header_f2 */

struct data_header_f2
{
	WORD ch_number;
	WORD byte_size;		/* TODO: check what this mean? */
	WORD num_samples;
	long sample_number;	/* for triggered data */
};				/* struct data_header_f2 */

#pragma pack()

struct datablock_infos
{
	long num_blocks;
	long num_samples;
	long *offsets;
	long *single_block_size;
};				/* struct datablock_infos */


struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	long hdr2_start;
	long ch_hdr_start;
	long data_start;

	struct global_header hdr;

	union
	{
		struct global_header_f0 f0;
		struct global_header_f1 f1;
		struct global_header_f2 f2;
	}
	hdr2;

	union
	{
		struct ch_header_f0 *f0;
		struct ch_header_f1 *f1;
		struct ch_header_f2 *f2;
	}
	ci;
	int di_ok;
	struct datablock_infos *di;

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
	RA_INFO_REC_CH_CENTER_VALUE_D,
	RA_INFO_REC_CH_MIN_UNIT_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int *_infos_obj = NULL;
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

void free_info_struct(struct info *i);

int get_data(struct info *i);
int read_global_header(struct info *i, FILE * fp);
void get_value(char *buf, char *value);
int read_format0(struct info *i, FILE * fp);
int read_format1(struct info *i, FILE * fp);
int read_ch_hdr1(struct ch_header_f1 *hdr, FILE * fp);
int read_format2(struct info *i, FILE * fp);

int read_datablock_infos(struct info *i);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw, DWORD *data_raw_high);
int block_start_end(struct info *i, int ch, size_t start, size_t num,
		     size_t *bl_start, size_t *bl_end);
size_t get_raw_0(struct info *i, int ch, size_t start, size_t num, size_t bl_start, size_t bl_end, double *data);
size_t get_raw_1(struct info *i, int ch, size_t start, size_t num, size_t bl_start, size_t bl_end, double *data, DWORD *data_raw);
size_t get_raw_2(struct info *i, int ch, size_t start, size_t num, size_t bl_start, size_t bl_end, double *data);
void get_data_file(struct info *i, char *fn);

#endif /* DASYLAB_H */
