/*----------------------------------------------------------------------------
 * cfs.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef CFS_H
#define CFS_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "cfs"
#define PLUGIN_DESC     gettext_noop("Handle signals saved using the CED Filing System (CFS)")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#ifndef WORD
#define WORD unsigned short
#endif  /* WORD */
#ifndef DWORD
#define DWORD unsigned long
#endif  /* DWORD */
#ifndef BYTE
#define BYTE unsigned char
#endif  /* BYTE */


/* variable- and data-types used in cfs */
#define CFS_INT1   0  /* char */
#define CFS_WRD1   1  /* byte */
#define CFS_INT2   2  /* short */
#define CFS_WRD2   3  /* WORD */
#define CFS_INT4   4  /* long */
#define CFS_RL4    5  /* float */
#define CFS_RL8    6  /* double */
#define CFS_LSTR   7  /* string */

/* possible kinds of data */
#define CFS_EQUALSPACED  0
#define CFS_MATRIX       1

/* flags */
#define CFS_NO_FLAG      0
#define CFS_FLAG7        1
#define CFS_FLAG6        2
#define CFS_FLAG5        4
#define CFS_FLAG4        8
#define CFS_FLAG3       16
#define CFS_FLAG2       32
#define CFS_FLAG1       64
#define CFS_FLAG0      128
#define CFS_FLAG15     256
#define CFS_FLAG14     512
#define CFS_FLAG13    1024
#define CFS_FLAG12    2048
#define CFS_FLAG11    4096
#define CFS_FLAG10    8192
#define CFS_FLAG9    16384
#define CFS_FLAG8    32768


#pragma pack(1)

struct cfs_global_hdr
{
	char id[8];
	BYTE file_name_len;
	char file_name[13];
	long file_size;
	char create_time[8];
	char create_date[8];
	short int num_ch;
	short int num_file_vars;
	short int num_data_sections_vars;
	short int file_hdr_size;
	short int data_section_hdr_size;
	long offset_last_data_section;
	WORD num_data_sections;
	WORD disk_block_rounding;
	BYTE comment_len;
	char comment[73];
	long offset_pointer_table;
	char reserved[40];
};  /* struct cfs_global_hdr */

struct cfs_ch_info
{
	BYTE name_len;
	char name[21];
	BYTE y_unit_len;
	char y_unit[9];
	BYTE x_unit_len;
	char x_unit[9];
	BYTE data_type;
	BYTE data_kind;
	short space_between_elems;
	short next_matrix_ch_data;
};  /* struct cfs_ch_info */

struct cfs_variable_info
{
	BYTE desc_len;
	char desc[21];
	short var_type;
	BYTE var_unit_len;
	char var_unit[9];
	short offset;
};  /* cfs_variable_info */

struct cfs_data_section_hdr
{
	long prev_data_section_hdr;
	long start_ch_data;
	long section_size;
	WORD flags;
	BYTE reserved[16];
};  /* cfs_data_section_hdr */

struct cfs_data_section_ch_info
{
	long start;
	long num_values;
	float y_scale;
	float y_offset;
	float x_inc;
	float x_offset;
};  /* struct cfs_data_section_ch_info */

#pragma pack()

struct cfs_data_section
{
	long abs_section_pos;

	struct cfs_data_section_hdr hdr;
	struct cfs_data_section_ch_info *ch_info;
	void **data_section_vars;

	long *ch_sample_start;
};  /* cfs_data_section */

struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	struct cfs_global_hdr global_hdr;
	struct cfs_ch_info *ch_info;
	struct cfs_variable_info *file_var_info;
	struct cfs_variable_info *data_section_var_info;
	void **file_vars;

	int data_section_read; 	/* flag if data-sections were read */
	struct cfs_data_section *data_section;
	long *data_section_offsets;

	/* at the moment only equalSpaced channels are supported;
	   this array maps the ch# coming from function-calls to
	   the ch# used in the cfs-file */
	int *lib_ch2file_ch;
	long num_ch_used;

	struct ra_est_ch_infos *ch_type_info;
	double mm_sec;
};  /* struct info */


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
int _num_infos_rec_gen =
sizeof (_infos_rec_gen) / sizeof (_infos_rec_gen[0]);

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
int _num_infos_rec_ch = sizeof (_infos_rec_ch) / sizeof (_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

/* int _infos_obj[] = { }; */
/* int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]); */
int *_infos_obj = NULL;

int _num_infos_obj = 0;


int pl_check_meas (const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas (ra_handle ra, const char *file, int fast);
int pl_close_meas (meas_handle mh);

int pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int pl_get_info_id(any_handle h, int id, value_handle vh);
int get_info_id (any_handle h, int id, value_handle vh);
double get_samplerate(struct info *i, int ch);
int get_info_rec (struct info *i, int id, value_handle vh);
int get_info_obj (struct info *i, int id, value_handle vh);


int pl_set_info(any_handle h, value_handle vh);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw, DWORD *data_raw_high);
size_t read_raw_data(struct info *i, FILE *fp, int ch, size_t start, size_t num,
		     double *data, DWORD *data_raw, DWORD *data_raw_high, long bl_start, long bl_end);


int get_data(struct info *i, int fast);
int read_global_header(struct info *i, FILE *fp);
int read_ch_info(struct info *i, FILE *fp);
int read_vars_info(struct cfs_variable_info **i, int n, FILE *fp);
int read_datasection_info(struct info *i);

void free_info_struct (struct info *i);

#endif /* CFS_H */


