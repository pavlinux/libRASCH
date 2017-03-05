/*----------------------------------------------------------------------------
 * mit_bih.h
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef MIT_BIH_H
#define MIT_BIH_H

#define PLUGIN_VERSION  "0.5.1"
#define PLUGIN_NAME     "mit-bih"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded using the MIT/BIH Data Format")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


struct date
{
	short int day;
	short int month;
	short int year;
};  /* struct date */


struct time
{
	short hour;  /* 0-23 */
	short min;
	short sec;
};  /* time */

struct signal_info
{
	char file_name[255];
	int format;
	int samples_per_frame;  /* follows 'x' */
	int skew;  /* follows ':' */
	int offset;  /* follows '+' */
	double adc_gain;  /* if not set use 200 */
	double adc_baseline;
	char unit[255];
	int num_bits;
	int zero_value;
	int initial_value;
	short checksum;
	int block_size;
	char description[255];
	int slot;		/* number of signal-slot in signal-file */
	int num_slots;		/* number of slots in signal-file */
	long slot_offset;	/* offset of current slot from frame-start */
	long slot_size;		/* number of samples in all slots per frame */
};  /* struct signal_info */


struct segment_info
{
	char rec_name[255];
	long num_samples;
};  /* struct segment_info */


struct record_info
{
	char rec_name[255];
	int num_segments;
	struct segment_info *segments;

	int num_signals;
	struct signal_info *signals;

	double samplerate;
	double counter_freq;
	int counter_freq_valid;
	double counter_value;
	int counter_value_valid;
	long num_samples;
	struct time base_time;
	struct date base_date;

	int num_infos;
	char **info_strings;
};  /* struct record_info */


struct annot_info
{
	long pos;
	int annot_orig;
	long annot_ra;
	int serial_no;		/* for AHA annotation files */

	char *text;		/* if it is a textual annotation, store text here */
	int annot_num;
	int annot_sub_type;
	int annot_ch;
	
	int use_it;		/* flag if this annotation should be set as an libRASCH event */
}; /* struct annot_info */


struct aha_annot_info
{
	unsigned char unused;
	unsigned char annot;
	unsigned long time;
	unsigned short serial_no;
	unsigned char mit_subtype;
	unsigned char mit_annot_code;
	char remark[6];
}; /* struct aha_annot_info */


struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	struct record_info ri;

	struct ra_est_ch_infos *ch_type_infos;
	double mm_sec;

	long num_ecg_annot;
	long num_ecg_use;
	struct annot_info *ecg;

	int num_text_annot;
	struct annot_info *text_annot;
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


int _infos_obj[] = {
	RA_INFO_OBJ_PATIENT_ID_C,
};
int _num_infos_obj = sizeof(_infos_obj) / sizeof(_infos_obj[0]);


int pl_check_meas(const char *name);
int pl_find_meas(const char *dir, struct find_meas *f);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);

int pl_close_meas(meas_handle mh);
void free_info_struct(struct info *i);

int get_data(struct info *i, int fast);

int handle_rec_line(char *data, struct info *i);
void init_info(struct info *i);

int read_signal_infos(struct info *i, FILE *fp);
void init_signal(struct signal_info *si);
int handle_signal_line(struct info *i, char *data, struct signal_info *si, char *last_file_name, int last_slot);

int read_info_lines(struct info *i, FILE *fp);

int get_channel_infos(struct info *i);

int get_next_line(char *buf, int size, FILE *fp);
char *get_sep(char *data, char *sep, char **next, const char *sep_allowed);
void string_to_date(struct date *d, char *s);
void string_to_time(struct time *d, char *s);

int get_annotations(struct info *i);
int get_annot_file_extension(struct info *i, char *annot_ext);
int read_wfdb_annot(struct info *i, long *num, struct annot_info **ai, FILE *fp, double samplerate);
int is_aha_format(unsigned char *b);
int read_aha_annot(long *num, struct annot_info **ai, FILE *fp, double samplerate);
int read_mit_annot(struct info *i, long *num, struct annot_info **ai, FILE *fp);

int get_annot_from_rri_files(struct info *i);
int check_if_getemed_rri(FILE *fp);
int read_getemed_rri(struct info *i, FILE *fp);

int estimate_samplenumber(struct info *i);

int pl_get_info_idx(any_handle h, int info_type, int index, value_handle vh);
int pl_get_info_id(any_handle h, int id, value_handle vh);
int get_info_id(any_handle h, int id, value_handle vh);
int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

int pl_set_info(meas_handle mh, value_handle vh);


size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info * i, int ch, size_t start, size_t num, double *data, DWORD *data_raw);
size_t read_raw_8bit(FILE *fp, size_t start, size_t num, short *buf, int diff);
size_t read_raw_10bit(FILE *fp, size_t start, size_t num, short *buf, int format_310);
size_t read_raw_12bit(FILE *fp, size_t start, size_t num, short *buf);
size_t read_raw_16bit(FILE *fp, size_t start, size_t num, short *buf, int offset, int big_endian);

/* WORD calc_checksum(BYTE *buf, size_t size); */

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

int pl_get_ev_value_all(prop_handle ph, unsigned long class_num, unsigned long prop_num, value_handle value, value_handle ch);
int get_all_beat_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);
int get_all_annot_events(struct eval_property *p, struct info *i, value_handle value, value_handle ch);

#endif /* MIT_BIH_H */
