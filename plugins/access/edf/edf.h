/*----------------------------------------------------------------------------
 * edf.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _EDF_H
#define _EDF_H

#define PLUGIN_VERSION  "0.4.0"
#define PLUGIN_NAME     "edf/edf+"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded using European Data Format(+)")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


struct timestamp
{
	short int year;
	short int mon;
	short int day;
	short int hour;
	short int min;
	short int sec;
};

#pragma pack(1)

struct edf_header_ascii
{
	char version[8];
	char pat_id[80];
	char rec_id[80];
	char rec_date[8];
	char rec_time[8];
	char num_hdr_bytes[8];
	char reserved[44];
	char num_recs[8];
	char duration[8];
	char num_ch[4];  /* = ns */
};


struct edf_header
{
	char version[9];
	char pat_id[81];
	char rec_id[81];
	struct timestamp rec_ts;
	long num_hdr_bytes;
	long num_recs;
	double duration;
	long num_ch;
}; /* struct edf_header */


#define MAX_FIELD_LEN   80

struct pat_info
{
	char id[MAX_FIELD_LEN];
	char gender[MAX_FIELD_LEN];
	char birthday[MAX_FIELD_LEN];
	char name[MAX_FIELD_LEN];
	char forename[MAX_FIELD_LEN];
}; /* struct pat_info */


struct rec_info
{
	char startdate[MAX_FIELD_LEN];
	char investigation_code[MAX_FIELD_LEN];
	char investigator_code[MAX_FIELD_LEN];
	char equipment_code[MAX_FIELD_LEN];	
}; /* struct rec_info */


struct channel_info
{
	char label[17];
	char transducer[81];
	char unit[9];
	double phys_min;
	double phys_max;
	long digital_min;
	long digital_max;
	char prefiltering[81];
	long num_samples;
	double samplerate;
	size_t start_filepos;

	/* informations needed to convert stored 2byte values back to float values */
	int float_values;
	double ymin;
	double a;
}; /* struct channel_info */


struct annotations
{
	double onset;
	double duration;
	char *text;
}; /* struct annotations */


struct session_info
{
	size_t start_blk;
	size_t end_blk;
}; /* struct session_info */


struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	struct edf_header hdr;

	struct pat_info pi;
	struct rec_info ri;

	int edf_plus;
	int non_contiguous;
	double max_samplerate;

	struct channel_info *ci;
	size_t record_size;

	long num_annot;
	struct annotations *annot;

	long num_sessions;
	struct session_info *session;

	double *start_time_rec_block;

	struct ra_est_ch_infos *ch_type_infos;
	double mm_sec;
}; /* struct info */

#pragma pack()

char *_supported_extensions[] = {
	"edf", "rec"
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

int _infos_obj[] = {
	RA_INFO_OBJ_PATIENT_ID_C,
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);


int pl_check_meas(const char *name);
int get_extension(struct info *i);

int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle mh, int minfo_type, int index, value_handle vh);
int pl_get_info_id(any_handle mh, int id, value_handle vh);
int get_info_id(any_handle mh, int id, value_handle vh);
int pl_set_info(any_handle mh, value_handle vh);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

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

size_t read_raw(struct info *i, rec_handle rh, int ch, size_t start, size_t num, float *buf_f, DWORD *buf_raw);
int read_data_block(struct info *i, size_t start_filepos, size_t len, int blk_num, WORD * buf, FILE * fp);

void free_info_struct(struct info *i);

int get_data(struct info *i);
void process_pat_id(struct info *i, char *id_text);
void process_rec_id(struct info *i, char *id_text);
char *get_sub_field(char *field, char *dest);
int get_number_of_records(struct info *i, FILE *fp);

int handle_annotations(struct info *i, int ch, FILE *fp);
char *get_annot(struct info *i, long record_block, char *p, long buf_size);
int combine_sessions(struct info *i);

int set_edf_sessions(struct ra_meas *meas);

int get_info_rec(any_handle h, struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);


#endif /* _EDF_H */
