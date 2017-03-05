/*----------------------------------------------------------------------------
 * portapres.h
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _PORTAPRES_H
#define _PORTAPRES_H

#define PLUGIN_VERSION  "0.2.0"
#define PLUGIN_NAME     "portapres"
#define PLUGIN_DESC     gettext_noop("Handle signals recorded with Portapres-System")
#define PLUGIN_TYPE     PLUGIN_ACCESS
#define PLUGIN_LICENSE  LICENSE_LGPL


#define MAX_NAME_LEN  100


/* some Portapres M2 struct's */
#pragma pack(1)
typedef struct
{
	char date_time[16];
	long xpos;
	long epos;
	long ypos;
	unsigned char misc[4];
}
PORTA_STARTSTOP_REC;

typedef struct
{
	union
	{
		long tim;
		unsigned char type[4];
	}
	u;
}
PORTA_TIME;

typedef struct
{
	PORTA_TIME time_type;
	union
	{
		short car[2];
		unsigned char bar[4];
	}
	u;
}
PORTA_EVENT_REC;
#pragma pack()



struct rec_file_info
{
	char *sig;
	char *unknown;		/* BF.mf-321-2ch */
	char *name;
	char *type;

	double sample_rate;

	int num_ch;
	int pres_ch;
	int height_ch;
	int mark_ch;

	char *pres_site;
	char *pres_lvls;

	char *RTF_cuf;
	char *co_method;

	char *config;
};				/* struct rec_file_info */

struct pat_info
{
	int age;
	int sex;		/* 1: male, 0: female (?) */
	int height;		/* in cm */
	int weight;		/* in kg */
	int BSA;		/* in m2 */
	int cal;		/* in % */

	char *id;
	char *name;
	char *procedure;
	char *info_4;
	char *info_5;
	char *info_6;
	char *info_7;
	char *info_8;
	char *info_9;
	char *info_10;
};				/* struct pat_info */

struct time_info
{
	char rec_date[11];
	char rec_time[9];

	/* time and date of conversion from m2-file to text-file */
	char conv_date[11];
	char conv_time[9];
};				/* struct time_info */

struct beat_info
{
	double offset;		/* from rec_time [sec] */

	int sys;		/* [mmHg] */
	int dias;		/* [mmHg] */
	int mean;		/* [mmHg] */

	int hr;			/* [bpm] */
	double rri;		/* [sec] */

	int vol;		/* volume [ml] */
	double co;		/* cardiac output [l/min] */
	double ejt;		/* ? [sec] */
	double tpr;		/* terminal peripheral resistance [MU] */
	int z0;			/* ? mMU */
	double cw;		/* ? [MU] */
	int puls;		/* ? [mmHg] */
	int height;		/* height relative to heart [mmHg] */

	int flags[8];
};				/* struct beat_info */
/*
  Time          Syst Dias Mean Rat   IBI Vol    CO    EJT    Tpr Artifact  Z0    Cw Puls  Hgt
  s             mmHg mmHg mmHg bpm     s  ml l/min      s     MU _TPSROD2 mMU    MU mmHg mmHg
  16,790         125   55   77  52 1,160 107  5,50  0,320  0,835 11001000  48  2,62   70   -6
*/

struct channel_desc
{
	char name[MAX_NAME_LEN];
	char unit[MAX_NAME_LEN];
	double amp_res;
	long num_samples;
	double samplerate;
}; /* struct channel_desc */


struct info
{
	ra_handle ra;

	char dir[MAX_PATH_RA];
	char name[MAX_PATH_RA];
	char ext[MAX_PATH_RA];

	int num_files;
	char **files;
	long size;

	struct rec_file_info ri;
	struct pat_info pi;
	struct time_info ti;

	int num_beats;
	struct beat_info *bi;

	int art;

	int raw_data_avail;
	char raw_data_file[MAX_PATH_RA];
	int num_channels;
	struct channel_desc *cd;

	struct ra_est_ch_infos *ch_type_infos;
	double mm_sec;
}; /* struct info */


int _infos_rec_gen[] = {
	RA_INFO_REC_GEN_TYPE_L,
	RA_INFO_REC_GEN_NAME_C,
	RA_INFO_REC_GEN_DESC_C,
/*	RA_INFO_REC_GEN_TIME_C,
	RA_INFO_REC_GEN_DATE_C, */
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
	RA_INFO_REC_CH_UNIT_C
};
/*int _num_infos_rec_ch = sizeof (_infos_rec_ch) / sizeof (_infos_rec_ch[0]);*/
int _num_infos_rec_ch = 0;

int *_infos_rec_dev = NULL;
int _num_infos_rec_dev = 0;

int *_infos_obj = NULL;
int _num_infos_obj = 0;


#ifdef WIN32
#define FREE_INFO(x)                   \
  __pragma(warning(push));             \
  __pragma(warning(disable:4127));     \
  do {                                 \
    if (x != NULL) free(x);            \
  } while(0)                           \
  __pragma(warning(pop))
#else
#define FREE_INFO(x)                   \
  do {                                 \
    if (x != NULL) free(x);            \
  } while(0)
#endif

int pl_check_meas(const char *name);

meas_handle pl_open_meas(ra_handle ra, const char *file, int fast);
int pl_close_meas(meas_handle mh);

int pl_get_info_idx(any_handle mh, int minfo_type, int index, value_handle vh);
int pl_get_info_id(any_handle mh, int id, value_handle vh);
int get_info_id(any_handle mh, int id, value_handle vh);
int pl_set_info(any_handle mh, value_handle vh);


void free_info_struct(struct info *i);

int get_data(struct info *i, int fast);

int read_m2_file(struct info *i);
int read_h00_file(struct info *i);
int read_b2b_file(struct info *i);
int read_raw_file(struct info *i);

int get_times(struct info *i);

int get_info_rec(struct info *i, int id, value_handle vh);
int get_info_obj(struct info *i, int id, value_handle vh);

size_t pl_get_raw(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
size_t pl_get_raw_unit(meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

size_t read_raw(struct info *i, int ch, size_t start, size_t num, double *data, DWORD *data_raw);
int read_data_block(struct info *i, size_t num, WORD * buf, FILE * fp);

#endif /* _PORTAPRES_H */
