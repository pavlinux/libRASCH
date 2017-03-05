/*----------------------------------------------------------------------------
 * ART.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _ART_H
#define _ART_H

/* -------------------- defines -------------------- */
#define PLUGIN_VERSION   "0.3.0"
#define PLUGIN_NAME      "ART"
#define PLUGIN_DESC      gettext_noop("Handle signals recorded for ART-Study")
#define PLUGIN_TYPE      PLUGIN_ACCESS
#define PLUGIN_LICENSE   LICENSE_LGPL


#define ART_RECORDING         0
#define PORTILAB_RECORDING    1
#define PORTAPRES_RECORDING   2

#define NUM_PORTILAB_CHANNELS  5

#define BUF_SIZE  1000

/* -------------------- structures -------------------- */
struct pat_info
{
	char *name;
	char *forename;
	char *birthday;
	char *gender;
	int age;
	int height;
	int weight;
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

	struct pat_info pi;

	meas_handle porti_meas;
	meas_handle porta_meas;

	int portapres_offset;

	int num_channels;
};


/* -------------------- global variables -------------------- */
/* pointer to the portilab and portapres plugins */
struct plugin_struct *_porti_plugin;
struct plugin_struct *_porta_plugin;

/* ---------- arrays with supported info ---------- */
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
	RA_INFO_REC_CH_UNIT_C,
	RA_INFO_REC_CH_CENTER_VALUE_D,
	RA_INFO_REC_CH_CENTER_SAMPLE_D,
	RA_INFO_REC_CH_MIN_UNIT_D,
	RA_INFO_REC_CH_MAX_UNIT_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int _infos_obj[] = {
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
	RA_INFO_OBJ_PERSON_AGE_L,
	RA_INFO_OBJ_PERSON_HEIGHT_L,
	RA_INFO_OBJ_PERSON_WEIGHT_L
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);


/* -------------------- prototypes -------------------- */

int pl_check_meas(const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

struct ra_rec_handle **pl_get_session_info(meas_handle mh, int *num_sessions);

int pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int pl_get_info_id(any_handle h, int id, value_handle vh);
int get_info_id(any_handle h, int id, value_handle vh);

int set_utf8(value_handle vh, const char *string);

int pl_set_info(any_handle h, int id, value_handle vh);

int write_single_pat_info(struct info *i, char **store, const char *group, const char *item, value_handle data);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, unsigned int ch, size_t start, size_t num, float *buf_f);
int read_data_block(struct info *i, unsigned int num, WORD * buf, FILE * fp);

void free_info_struct(struct info *i);

int get_data(struct info *i);
int open_single_meas(struct info *i);
int read_patinfo(struct info *i);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_rec_ch(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);


#endif /* _ART_H */
