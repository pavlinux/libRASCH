/**
 * \file ra_defines.h
 *
 * The header file contains all definitions needed to use the libRASCH API
 * in other programs. For libRASCH plugins, see the other header files.
 *
 * \author Raphael Schneider (librasch@gmail.com)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2009, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _RA_DEFINES_H
#define _RA_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#ifndef _LIBRASCH_BUILD
#define LIBRAAPI __declspec(dllimport)
#else
#define LIBRAAPI __declspec(dllexport)
#endif
#else
#define LIBRAAPI
#endif


typedef void *ra_handle;
typedef void *value_handle;
typedef void *meas_handle;
typedef void *rec_handle;
typedef void *ra_find_handle;
typedef void *eval_handle;
typedef void *class_handle;
typedef void *prop_handle;
typedef void *sum_handle;
typedef void *plugin_handle;
typedef void *proc_handle;
typedef void *view_handle;
typedef void *any_handle;	/* can be a handle of any type (up to now it is used for
				   proc_handle and view_handle in ra_gui_call() */


#define RA_HANDLE_LIB            1
#define RA_HANDLE_MEAS           2
#define RA_HANDLE_REC            3
#define RA_HANDLE_FIND           4
#define RA_HANDLE_EVAL           5
#define RA_HANDLE_EVAL_CLASS     6
#define RA_HANDLE_EVAL_PROP      7
#define RA_HANDLE_EVAL_SUMMARY   8
#define RA_HANDLE_PLUGIN         9
#define RA_HANDLE_PROC          10	
#define RA_HANDLE_VIEW          11
#define RA_HANDLE_VALUE         12


#ifndef MAX_PATH_RA
#define MAX_PATH_RA      4096
#endif

#ifndef RA_PI
#define RA_PI 3.14159265358979323846
#endif /* RA_PI */

/* SLASH and BACK-SLASH are ASCII char's therefore no special
   UTF-8 char's are necessary */
#ifdef WIN32
#define SEPARATOR '\\'
#endif
#ifdef LINUX
#define SEPARATOR '/'
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short int WORD;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifdef WIN32
#define RA_STRICMP  _stricmp
#define RA_STRNICMP _strnicmp
#endif
#ifdef LINUX
#define RA_STRICMP  strcasecmp
#define RA_STRNICMP strncasecmp
#endif

#define RA_VALUE_NAME_MAX    50
#define RA_VALUE_DESC_MAX   250
#define RA_VALUE_VALUE_MAX 2000	/* TODO: check what this is for */

#define RA_VALUE_TYPE_NONE           0
#define RA_VALUE_TYPE_SHORT          1
#define RA_VALUE_TYPE_LONG           2
#define RA_VALUE_TYPE_DOUBLE         3
#define RA_VALUE_TYPE_CHAR           4
#define RA_VALUE_TYPE_VOIDP          5
#define RA_VALUE_TYPE_SHORT_ARRAY    6
#define RA_VALUE_TYPE_LONG_ARRAY     7
#define RA_VALUE_TYPE_DOUBLE_ARRAY   8
#define RA_VALUE_TYPE_CHAR_ARRAY     9
#define RA_VALUE_TYPE_VOIDP_ARRAY   10
#define RA_VALUE_TYPE_VH_ARRAY      11


/* defines describing the type of a measurement */
#define RA_MEAS_TYPE_UNKNOWN       0
#define RA_MEAS_TYPE_ECG_HOLTER    1
#define RA_MEAS_TYPE_ECG_REST      2
#define RA_MEAS_TYPE_ECG_STRESS    3


/* defines for identification of recordings */
#define RA_REC_TYPE_UNKNOWN            0
#define RA_REC_TYPE_MIXED              1	/* recording has sub-recordings of different types */
#define RA_REC_TYPE_TIMESERIES        10	/* recording is a timeseries (e.g. ecg) */
#define RA_REC_TYPE_IMAGE             11	/* recording is an image (e.g. x-ray image) */
#define RA_REC_TYPE_MOVIE             12	/* recording is a movie (e.g. cath-lab examination) */
#define RA_REC_TYPE_DAQ_TIMESERIES   100	/* an online daq device recording a timeseries */


/* defines for identification of channel-types */
#define RA_CH_TYPE_UNKNOWN  0
/* timeseries channels */
#define RA_CH_TYPE_ECG        1	/* electrocardiogram */
#define RA_CH_TYPE_RR         2	/* continues bloodpressure signal */
#define RA_CH_TYPE_RESP       3	/* continues respiration signal */
#define RA_CH_TYPE_TEMP       4	/* temperature */
#define RA_CH_TYPE_TEMP_C     5	/* temperature in Celsius (FIXME: why different degree units?) */
#define RA_CH_TYPE_TEMP_F     6	/* temperature in Fahrenheit */
#define RA_CH_TYPE_ACTIVITY   7	/* activity */
#define RA_CH_TYPE_EEG        8 /* electroencephalogram */
#define RA_CH_TYPE_EMG        9 /* ??? */
#define RA_CH_TYPE_CTG_FHR   10 /* fetal heart rate in the CTG */
#define RA_CH_TYPE_CTG_UC    11 /* uterine contractions in the CTG */
#define RA_CH_TYPE_ANGLE     12 /* angle in degree */
#define RA_CH_TYPE_EGM       13 /* electrogram (intra-cardiac signal) */
#define RA_CH_TYPE_EGM_RA    14 /* electrogram right atrium */
#define RA_CH_TYPE_EGM_LA    15 /* electrogram left atrium */
#define RA_CH_TYPE_EGM_RV    16 /* electrogram right ventricle */
#define RA_CH_TYPE_EGM_LV    17 /* electrogram left ventricle */
/* picture channels */
/* movie channels */

/* defines for sample storage types */
#define RA_CH_SAMPLE_TYPE_INT      0  /* samples are stored as signed integer values (size is given in RA_INFO_REC_CH_NUM_BITS_L) */
#define RA_CH_SAMPLE_TYPE_WORD     1  /* samples are stored as unsigned integer values (size is given in RA_INFO_REC_CH_NUM_BITS_L) */
#define RA_CH_SAMPLE_TYPE_FLOAT    2  /* samples are stored as float values (4 bytes) */
#define RA_CH_SAMPLE_TYPE_FLOAT2   3  /* samples are float values but stored in 2 bytes; values are log-scaled;
					 up to now only the EDF(+) format supports it (see http://www.hsr.nl/edf/edffloat.htm) */
#define RA_CH_SAMPLE_TYPE_DOUBLE   4  /* samples are stored as double values (8 bytes) */


/* defines for raw-data processing tasks */
#define RA_RAW_PROC_RM_POWERLINE_NOISE    0 /* remove powerline noise (1 optional long-option: 1=force filter) */
#define RA_RAW_PROC_RM_MEAN               1 /* remove mean-value (no options) */


/* defines for evaluations */
#define EVAL_MAX_NAME      64
#define EVAL_MAX_DESC     256
#define EVAL_MAX_UNIT      64

#define PLUGIN_DAQ     16	/* access data acquisition boards */

/* possible type's of plugins */
#define PLUGIN_ACCESS   1	/* do file access */
#define PLUGIN_PROCESS  2	/* process data */
#define PLUGIN_GUI      4	/* user interface modules (mostly dialog boxes) */
#define PLUGIN_VIEW     8	/* views on data */


/* ------------------------------ types ------------------------------ */

/**
 * \struct ra_value
 * \brief used to exchange values and to receive info values from libRASCH
 * \param <name> name of value
 * \param <name_locale> name of value encoded in local charset (only for internal use, DO NOT USE IT)
 * \param <desc> short description of value
 * \param <desc_locale> short description of value encoded in local charset (only for internal use, DO NOT USE IT)
 * \param <utype> type of value (short, long, char *, ...)
 * \param <num_values> if array-type, number of elements
 * \param <value_is_valid> flag if value is valid
 * \param <value> union which holds value
 * \param <value_locale> union which holds string-value in their original encoding
 * \param <id> id of info
 * \param <can_be_changed> flag if value can be changed (if returned from a signal-file)
 * \param <number> a general number variable
 */
struct ra_value
{
	unsigned short handle_id;

	char name[RA_VALUE_NAME_MAX];
	char name_locale[RA_VALUE_NAME_MAX];
	char desc[RA_VALUE_DESC_MAX];
	char desc_locale[RA_VALUE_DESC_MAX];
	int utype;		/* values see above RA_VALUE_TYPE_* */
	unsigned long num_values;	/* if array-type is used */
	int value_is_valid;	/* flag if value is valid (at the moment it is used only by process-results) */
	union
	{
		short s;
		long l;
		double d;
		char *c;
		void *vp;

		short *sp;
		long *lp;
		double *dp;
		char **cp;
		void **vpp;
		value_handle *vhp;
	} value;

	/* when string or string-array, store these in "original" encoding */
	union
	{
		char *c;
		char **cp;
	} value_locale;

	int id;		 /* id of info (see enum ra_info_ids below) */
	int can_be_changed;	/* flag if value can be saved */

	long number;
};  /* struct ra_value */


/**
 * \struct ra_find_struct
 * \brief infos about a found signal
 * \param <name> file name of signal
 * \param <plugin> name of the access plugin handling the file
 */
struct ra_find_struct
{
	char name[MAX_PATH_RA];
	char plugin[RA_VALUE_NAME_MAX];
};  /* struct ra_find_struct */


/* ------------------------------ infos ------------------------------ */
enum ra_info_ids
{
	RA_INFO_NONE = -1,

	/* -------------------- infos about library -------------------- */
	RA_INFO_LIB_START = 0,
	RA_INFO_NUM_PLUGINS_L,
	RA_INFO_VERSION_C,
	RA_INFO_LIB_END,

	/* -------------------- infos about measurement -------------------- */
	RA_INFO_MEASUREMENT_START = 1000,
	RA_INFO_NUM_SESSIONS_L,
	RA_INFO_NUM_OBJ_INFOS_L,
	RA_INFO_NUM_REC_GEN_INFOS_L,
	RA_INFO_NUM_REC_DEV_INFOS_L,
	RA_INFO_NUM_REC_CH_INFOS_L,
	RA_INFO_NUM_EVAL_INFOS_L,
	RA_INFO_MAX_SAMPLERATE_D,  /* maximum samplerate used in the recording 
				       (this samplerate will be used for sampleunits
				       in positions in evaluations) */
	RA_INFO_CH_XSCALE_D,  /* factor for a specific channel to scale the positions from
				  evaluations to positions in the channel (needed for measurements
				  with different samplerates) */
	RA_INFO_SIZE_L,
	RA_INFO_FILES_CA,
	RA_INFO_PATH_C,	/* full path of measurement incl. measurement name */
	RA_INFO_DIR_L,	/* flag if measurement was saved in directory */
	RA_INFO_TYPE_L, /* type of the measurement (e.g. Holter ECG), see RA_MEAS_TYPE_* */
	RA_INFO_MEASUREMENT_END,
	
	/* -------------------- infos about the measurement object -------------------- */
	RA_INFO_OBJECT_START = 2000,
	RA_INFO_OBJECT = RA_INFO_OBJECT_START,
	/* ---------- mobj is a person ---------- */
	RA_INFO_OBJ_PERSON_START,
	RA_INFO_OBJ_PERSON_NAME_C,
	RA_INFO_OBJ_PERSON_FORENAME_C,
	RA_INFO_OBJ_PERSON_BIRTHDAY_C,
	RA_INFO_OBJ_PERSON_GENDER_C,
	RA_INFO_OBJ_PERSON_AGE_L,
	RA_INFO_OBJ_PERSON_HEIGHT_L,
	RA_INFO_OBJ_PERSON_WEIGHT_L,
	RA_INFO_OBJ_PERSON_STREET_C,
	RA_INFO_OBJ_PERSON_CITY_C,
	RA_INFO_OBJ_PERSON_POSTALCODE_C,
	RA_INFO_OBJ_PERSON_COUNTRY_C,
	RA_INFO_OBJ_PERSON_ADDRESS_C,
	RA_INFO_OBJ_PERSON_PHONE1_C,
	RA_INFO_OBJ_PERSON_PHONE2_C,
	RA_INFO_OBJ_PERSON_PHONE_MOBILE_C,
	RA_INFO_OBJ_PERSON_FAX_C,
	RA_INFO_OBJ_PERSON_EMAIL_C,
	RA_INFO_OBJ_PERSON_WEBSITE_C,
	RA_INFO_OBJ_PERSON_COMMENT_C,
	RA_INFO_OBJ_PERSON_END,
	/* ---------- mobj is a patient ---------- */
	/* (for person related infos use RA_INFO_OBJ_PERSON_*) */
	RA_INFO_OBJ_PATIENT_START,
	RA_INFO_OBJ_PATIENT_ID_C,
	RA_INFO_OBJ_PATIENT_REASON_C,
	RA_INFO_OBJ_PATIENT_DIAGNOSIS_C,
	RA_INFO_OBJ_PATIENT_THERAPY_C,
	RA_INFO_OBJ_PATIENT_MEDICATION_C,
	RA_INFO_OBJ_PATIENT_HOSPITAL_C,
	RA_INFO_OBJ_PATIENT_DEPARTMENT_C,
	RA_INFO_OBJ_PATIENT_DOCTOR_C,
	RA_INFO_OBJ_PATIENT_EXAMINER_C,
	RA_INFO_OBJ_PATIENT_END,
	/* ---------- mobj is a pregnant woman ---------- */
	/* (for person related infos use RA_INFO_OBJ_PERSON_*) */
	/* (for patient related infos use RA_INFO_OBJ_PATIENT_*) */
	RA_INFO_OBJ_PREG_WOMAN_START,
	RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L,
	RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C,
	RA_INFO_OBJ_PREG_WOMAN_END,
	RA_INFO_OBJECT_END,

	/* -------------------- infos about the recording -------------------- */
	RA_INFO_RECORDING_START = 3000,
	RA_INFO_REC_GENERAL_START,
	RA_INFO_REC_GENERAL = RA_INFO_REC_GENERAL_START,
	RA_INFO_REC_GEN_TYPE_L,	/* type of recording (see RA_REC_TYPE_*) */
	RA_INFO_REC_GEN_NAME_C,	/* name of recording */
	RA_INFO_REC_GEN_DESC_C,
	RA_INFO_REC_GEN_COMMENT_C,
	RA_INFO_REC_GEN_TIME_C,	/* start time of recording (hh:mm:ss) */
	RA_INFO_REC_GEN_DATE_C,	/* start date of recording (dd.mm.yyyy) */
	RA_INFO_REC_GEN_DURATION_SEC_D,
	RA_INFO_REC_GEN_NUMBER_L,	/* number of recording (0: main recording) */
	RA_INFO_REC_GEN_NUM_SUB_REC_L,	/* number of sub-recordings */
	RA_INFO_REC_GEN_NUM_DEVICES_L,	/* number of devices used for recording */
	RA_INFO_REC_GEN_NUM_CHANNEL_L,	/* number of channels */
	RA_INFO_REC_GEN_OFFSET_L,	/* if sub-recording: num of msec time shift between
				   the sub-recordings (first sub-rec. is 0) */
	RA_INFO_REC_GEN_PATH_C,	/* when rec-number#0: same as RA_INFO_PATH_C */
	RA_INFO_REC_GEN_DIR_L,	/* when rec-number#0: same as RA_INFO_DIR_L */
	RA_INFO_REC_GEN_MM_SEC_D,	/* mm per second (hint for displaing signal) */
	RA_INFO_REC_GEN_IS_SAMPLING_L, /* flag if device is recording (start/stop it with ra_info_set()) */
	RA_INFO_REC_GENERAL_END,
	/* ---------- device infos ---------- */
	RA_INFO_REC_DEVICE_START,
	RA_INFO_REC_DEVICE = RA_INFO_REC_DEVICE_START,
	RA_INFO_REC_DEV_HW_NAME_C,
	RA_INFO_REC_DEV_HW_MANUFACTURER_C,
	RA_INFO_REC_DEV_HW_SERIAL_NO_C,
	RA_INFO_REC_DEV_HW_VERSION_C,
	RA_INFO_REC_DEV_SW_NAME_C,
	RA_INFO_REC_DEV_SW_MANUFACTURER_C,
	RA_INFO_REC_DEV_SW_SERIAL_NO_C,
	RA_INFO_REC_DEV_SW_VERSION_C,
	RA_INFO_REC_DEVICE_END,
	/* ---------- channel infos ---------- */
	RA_INFO_REC_CHANNEL_START,
	RA_INFO_REC_CHANNEL = RA_INFO_REC_CHANNEL_START,
	RA_INFO_REC_CH_NAME_C,
	RA_INFO_REC_CH_DESC_C,
	RA_INFO_REC_CH_NUM_SAMPLES_L,
	RA_INFO_REC_CH_START_SAMPLE_L,
	RA_INFO_REC_CH_LAST_SAMPLE_L,
	RA_INFO_REC_CH_SAMPLERATE_D,
	RA_INFO_REC_CH_NUM_BITS_L,
	RA_INFO_REC_CH_AMP_RESOLUTION_D,
	RA_INFO_REC_CH_SAMPLE_TYPE_L,   /* storage type of the sample values (e.g. integer, real) */
	RA_INFO_REC_CH_UNIT_C,
	RA_INFO_REC_CH_CENTER_VALUE_D,	/* value[unit] signal is centered */
	RA_INFO_REC_CH_CENTER_SAMPLE_D,	/* center value as sample-value */
	RA_INFO_REC_CH_MIN_ADC_D,	/* minimum value of ADC amp-levels */
	RA_INFO_REC_CH_MAX_ADC_D,	/* maximum value of ADC amp-levels */
	RA_INFO_REC_CH_MIN_UNIT_D,	/* minimum value in units */
	RA_INFO_REC_CH_MAX_UNIT_D,	/* maximum value in units */
	RA_INFO_REC_CH_MM_UNIT_D,	/* mm per unit (hint for displaing signal) */
	RA_INFO_REC_CH_TYPE_L,	/* type of channel (see above RA_CH_TYPE_*) */
	RA_INFO_REC_CH_DO_IGNORE_VALUE_L,
	RA_INFO_REC_CH_IGNORE_VALUE_D,
	RA_INFO_REC_CHANNEL_END,
	RA_INFO_RECORDING_END,

	/* -------------------- infos about evalution -------------------- */
	RA_INFO_EVALUATION = 4000,
	RA_INFO_EVAL_START,
	RA_INFO_EVAL_NAME_C,	  /* name of the evaluation (optional) */
	RA_INFO_EVAL_DESC_C,	  /* description of the evaluation (optional) */
	RA_INFO_EVAL_ADD_TS_C,	  /* timestamp when eval was added */
	RA_INFO_EVAL_MODIFY_TS_C, /* timestamp when eval was last modified */
	RA_INFO_EVAL_USER_C,	  /* user who added eval */
	RA_INFO_EVAL_HOST_C,	  /* machine on which eval was added */
	RA_INFO_EVAL_PROG_C,	  /* program which added eval */
	RA_INFO_EVAL_ORIGINAL_L,  /* evaluation done in recording system */
	RA_INFO_EVAL_DEFAULT_L,	  /* evaluation which should be used */
	RA_INFO_EVAL_END,
	/* ---------- infos about event-classes ---------- */
	RA_INFO_CLASS_START,
	RA_INFO_CLASS_ASCII_ID_C, /* id-string of event-class */
	RA_INFO_CLASS_NAME_C,	  /* name of the event class (optional) */
	RA_INFO_CLASS_DESC_C,	  /* description of event class (optional) */
	RA_INFO_CLASS_EV_NUM_L,	  /* number of events in the event class */
	RA_INFO_CLASS_END,
	/* ---------- infos about event-properties ---------- */
	RA_INFO_PROP_START,
	RA_INFO_PROP_ASCII_ID_C,   /* id-string of event-property */
	RA_INFO_PROP_VALTYPE_L,	   /* type of the event-property values (short, long, double, char) */
	RA_INFO_PROP_NAME_C,	   /* name of event-property (optional) */
	RA_INFO_PROP_DESC_C,	   /* description of event property (optional) */
	RA_INFO_PROP_UNIT_C,	   /* unit of the property (optional) */
	RA_INFO_PROP_HAS_MINMAX_L, /* flag if property has min-/max-values (i.e. normal value range) (optional) */
	RA_INFO_PROP_MIN_D,	   /* normal min-value of the property (optional) */
	RA_INFO_PROP_MAX_D,	   /* normal max-value of the property (optional) */
	RA_INFO_PROP_IGNORE_VALUE_D, /* value which should be ignored (if not available,
					an invalid value-handle will be returned) */
	RA_INFO_PROP_END,
	/* ---------- infos about event-summaries ---------- */
	RA_INFO_SUM_START,	   /* start of event-summary specific info-ids */
	RA_INFO_SUM_ASCII_ID_C,    /* id-string of event-summary */
	RA_INFO_SUM_NAME_C,	   /* name of the event summary */
	RA_INFO_SUM_DESC_C,	   /* description of the event summary */
	RA_INFO_SUM_NUM_DIM_L,     /* number of dimensions used for the summary */
	RA_INFO_SUM_DIM_UNIT_C,    /* dimension unit */
	RA_INFO_SUM_DIM_NAME_C,    /* dimension name */
	RA_INFO_SUM_NUM_CH_L,	   /* number of channels where a summary where got */
	RA_INFO_SUM_CH_NUM_L,	   /* channel-number used */
	RA_INFO_SUM_CH_FIDUCIAL_L, /* offset of fiducial point (-1 if none) */
	RA_INFO_SUM_END,	   /* end of event-summary specific info-ids */
	RA_INFO_EVALUATION_END,

	/* -------------------- infos about plugins -------------------- */
	RA_INFO_PLUGIN_START = 5000,
	RA_INFO_PL_NAME_C,	/* name of plugin, used for identification */
	RA_INFO_PL_DESC_C,	/* description of plugin */
	RA_INFO_PL_FILE_C,	/* filename with path of plugin */
	RA_INFO_PL_USE_IT_L,	/* flag if plugin should be used */
	RA_INFO_PL_TYPE_L,	/* type of plugin (see PLUGIN_XXX above) */
	RA_INFO_PL_VERSION_C,	/* version of plugin */
	RA_INFO_PL_BUILD_TS_C,	/* timestamp when plugin was build */
	RA_INFO_PL_LICENSE_L,	/* license-id used for plugin (see LICENSE_* defines in ra_priv.h) */
	RA_INFO_PL_NUM_OPTIONS_L,	/* number of options available in the plugin */
	RA_INFO_PL_OPT_NAME_C,	/* short name of option */
	RA_INFO_PL_OPT_DESC_C,	/* description of option */
	RA_INFO_PL_OPT_TYPE_L,	/* type of option (long, double, char) */
	RA_INFO_PL_NUM_RESULTS_L,	/* number of values returned from plugin */
	RA_INFO_PL_RES_NAME_C,	/* short name of result (can be used as table header) */
	RA_INFO_PL_RES_DESC_C,	/* description of result */
	RA_INFO_PL_RES_TYPE_L,	/* type of result (long, double, char) */
	RA_INFO_PL_RES_DEFAULT_L, /* flag if result belongs to default-values (=1) */
	RA_INFO_PLUGIN_END,

	/* -------------------- infos about sessions -------------------- */
	RA_INFO_SESSION_START = 6000,
	RA_INFO_SES_NAME_C,	/* name of session */
	RA_INFO_SES_DESC_C,	/* description of session */
	RA_INFO_SESSION_END,

	/* -------------------- infos about processing results -------------------- */
	RA_INFO_PROCESSING_START = 7000,
	RA_INFO_PROC_NUM_RES_SETS_L, /* number of available result sets */
	RA_INFO_PROC_NUM_RES_L,      /* number of available results per set */
	RA_INFO_PROCESSING_END
}; /* enum ra_info_ids */


/* ------------------------------ predefined evaluation infos ------------------------------ */
/* ---------- event classes ---------- */
enum event_class_ids
{
	EVENT_CLASS_HEARTBEAT,	         /* all infos about heart beats */
	EVENT_CLASS_RR_CALIBRATION,      /* all infos about calibration sequences in bloodpressure measurements */
	EVENT_CLASS_ANNOT,	         /* annotations/comments */
	EVENT_CLASS_ARRHYTHMIA,	         /* arrhythmic events */
	EVENT_CLASS_ARRHYTHMIA_ORIGINAL, /* arrhythmic events from original evaluation */
	EVENT_CLASS_UTERINE_CONTRACTION, /* infos about uterine contraction */
	EVENT_CLASS_RESPIRATION,         /* respiration related data */
	EVENT_CLASS_EGM_MARKER           /* EGM-marker related data */
}; /* enum event_class_ids */

/* ---------- event properties ---------- */
enum event_prop_ids
{
	/* -------------------- values for QRS complexes -------------------- */
	/* channel independent values */
	EVENT_PROP_QRS_TEMPLATE,
	EVENT_PROP_QRS_TEMPLATE_CORR,
	EVENT_PROP_QRS_POS,
	EVENT_PROP_QRS_ANNOT,
	EVENT_PROP_QRS_CH,
	EVENT_PROP_QRS_TEMPORAL,
	/* ECG related infos */
	EVENT_PROP_ECG_FLAGS,
	/* morphology values for each beat in each channel*/
	EVENT_PROP_ECG_NOISE,
	EVENT_PROP_ECG_P_TYPE,
	EVENT_PROP_ECG_QRS_TYPE,
	EVENT_PROP_ECG_T_TYPE,
	EVENT_PROP_ECG_U_TYPE,
	EVENT_PROP_ECG_MORPH_FLAGS,
	EVENT_PROP_ECG_P_START_OFFSET,
	EVENT_PROP_ECG_P_PEAK_1_OFFSET,
	EVENT_PROP_ECG_P_PEAK_2_OFFSET,
	EVENT_PROP_ECG_P_END_OFFSET,
	EVENT_PROP_ECG_QRS_START_OFFSET,
	EVENT_PROP_ECG_Q_PEAK_OFFSET,
	EVENT_PROP_ECG_R_PEAK_OFFSET,
	EVENT_PROP_ECG_S_PEAK_OFFSET,
	EVENT_PROP_ECG_RS_PEAK_OFFSET,
	EVENT_PROP_ECG_QRS_END_OFFSET,
	EVENT_PROP_ECG_T_START_OFFSET,
	EVENT_PROP_ECG_T_PEAK_1_OFFSET,
	EVENT_PROP_ECG_T_PEAK_2_OFFSET,
	EVENT_PROP_ECG_T_END_OFFSET,
	EVENT_PROP_ECG_U_START_OFFSET,
	EVENT_PROP_ECG_U_PEAK_1_OFFSET,
	EVENT_PROP_ECG_U_PEAK_2_OFFSET,
	EVENT_PROP_ECG_U_END_OFFSET,
	/* morphology values which should be used for further analysis (only one value for each beat) */
	EVENT_PROP_ECG_P_WIDTH,
	EVENT_PROP_ECG_QRS_WIDTH,
	EVENT_PROP_ECG_T_WIDTH,
	EVENT_PROP_ECG_PQ,
	EVENT_PROP_ECG_QT,
	EVENT_PROP_ECG_QTC,
	EVENT_PROP_ECG_QTA,

	/* -------------------- values for RR intervals -------------------- */
	EVENT_PROP_RRI,
	EVENT_PROP_RRI_ANNOT,
	EVENT_PROP_RRI_REFVALUE,
	EVENT_PROP_RRI_NUM_REFVALUE,

	/* -------------------- values for respiration signals -------------------- */
	EVENT_PROP_ECG_RESP_PHASE,
	EVENT_PROP_RESP_CHEST_MEAN_RRI,
	EVENT_PROP_RESP_CHEST_MEAN_IBI,
	
	/* -------------------- values for bloodpressure -------------------- */
	EVENT_PROP_RR_TEMPLATE,
	EVENT_PROP_RR_SYSTOLIC,
	EVENT_PROP_RR_SYSTOLIC_POS,
	EVENT_PROP_RR_DIASTOLIC,
	EVENT_PROP_RR_DIASTOLIC_POS,
	EVENT_PROP_RR_MEAN,
	EVENT_PROP_RR_FLAGS,
	EVENT_PROP_RR_IBI,
	EVENT_PROP_RR_DPDT_MIN,
	EVENT_PROP_RR_DPDT_MAX,
	EVENT_PROP_RR_DPDT_MIN_POS,
	EVENT_PROP_RR_DPDT_MAX_POS,
	EVENT_PROP_RR_DPDT_MIN_PRESSURE,
	EVENT_PROP_RR_DPDT_MAX_PRESSURE,

	/* -------------------- values for bloodpressure calibration -------------------- */
	EVENT_PROP_RR_CALIBRATION_INFO,

	/* -------------------- values for uterine contraction -------------------- */
	EVENT_PROP_UC_MAX_POS,
	EVENT_PROP_UC_TEMPLATE,
	EVENT_PROP_UC_TEMPLATE_CORR,

	/* -------------------- values for comments/annotations -------------------- */
	EVENT_PROP_ANNOT,

	/* ---------- values for arryhthmic events (set in the ecg libRASCH plugin) ---------- */
	EVENT_PROP_ARR_TYPE,
	EVENT_PROP_ARR_HR,
	EVENT_PROP_ARR_NUM_QRS,
	/* ---------- values for arryhthmic events from original evaluation ---------- */
	EVENT_PROP_ARR_ORIG_TYPE,
	EVENT_PROP_ARR_ORIG_HR,
	EVENT_PROP_ARR_ORIG_NUM_QRS,

	/* -------------------- values for respiration -------------------- */
	EVENT_PROP_RESP_POS,
	EVENT_PROP_RESP_PHASE,

	/* -------------------- values for EGM marker channels -------------------- */
	EVENT_PROP_EGM_MARKER_POS,
	EVENT_PROP_EGM_MARKER_TYPE
}; /* enum event_prop_ids */

#ifdef __cplusplus
}
#endif

#endif /* _RA_DEFINES_H */
