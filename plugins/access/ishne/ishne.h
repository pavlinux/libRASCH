/*----------------------------------------------------------------------------
 * ishne.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef ISHNE_H
#define ISHNE_H

#define PLUGIN_VERSION  "0.3.0"
#define PLUGIN_NAME     "ishne-holter"
#define PLUGIN_DESC     gettext_noop("Handle signals saved using ISHNE standard output for Holter ECG")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#define SYS_NONE   0
#define SYS_ELA    1


#pragma pack(1)

struct ishne_date
{
	short int day;
	short int month;
	short int year;
};  /* struct ishne_date */


struct ishne_time
{
	short int hour;  /* 0-23 */
	short int min;
	short int sec;
};  /* struct ishne_time */


struct header
{
	long var_block_size;
	long num_samples;
	long var_block_offset;
	long ecg_offset;
	short int version;
	char subject_forename[40];
	char subject_name[40];
	char subject_id[20];
	short int sex;  /* 0:unknown, 1:male, 2:female */
	short int race;  /* 0:unknown, 1:Caucasian, 2:Black, 3:Oriental, 4-9:reserved*/
	struct ishne_date birthday;
	struct ishne_date rec_date;
	struct ishne_date output_date;
	struct ishne_time rec_time;
	short int num_leads;
	short int lead_specification[12];
	short int lead_qualitay[12];
	short int amp_resolution[12];  /* # of nV */
	short int pacemaker_code;
	char recorder_type[40];  /* analog/digital */
	short int samplerate;
	char prop_of_ecg[80];
	char copyright[80];
	char reserved[88];
};  /*struct header */

#pragma pack()


struct beat_info
{
	long pos;
	long annotation;
};  /* struct beat_info */

struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	char file_id[8];
	short int checksum;

	struct header hdr;

	char *manufacturer_specific;

	int holter_system;
	int broken_ela_format;

	int beats_ok;
	long num_beats;
	struct beat_info *beats;
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
	RA_INFO_REC_GEN_MM_SEC_D,
	RA_INFO_REC_CH_CENTER_VALUE_D,
	RA_INFO_REC_CH_CENTER_SAMPLE_D,
	RA_INFO_REC_CH_MM_UNIT_D,
	RA_INFO_REC_CH_TYPE_L,
};
int _num_infos_rec_ch = sizeof(_infos_rec_ch) / sizeof(_infos_rec_ch[0]);

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int _infos_obj[] = {
	RA_INFO_OBJ_PATIENT_ID_C,
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
/*	RA_INFO_OBJ_PERSON_RACE_C, */
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

int get_data(struct info *i);
void handle_endian_ishne_date(struct ishne_date *d);
void handle_endian_ishne_time(struct ishne_time *t);
int find_rri_files(struct info *i);
int read_ela_rri(struct info *i, char *ext);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

WORD calc_checksum(BYTE *buf, size_t size);

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
int pl_get_ev_value(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle value, value_handle ch);

int pl_get_ev_value_all(prop_handle ph, unsigned long class_num, unsigned long prop_num, value_handle value, value_handle ch);
int get_all_beat_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);
int get_all_annot_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);

#endif /* ISHNE_H */
