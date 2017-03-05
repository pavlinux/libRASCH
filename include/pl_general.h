/**
 * \file pl_general.h
 *
 * The header file contains meta-infos about infos which are handled by
 * libRASCH.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef PL_GENERAL_H
#define PL_GENERAL_H

#include <ra_priv.h>
#include <ra.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \struct meta_info
 * \brief meta-informations about informations handled by libRASCH
 * \param <id> id of the info (see RA_INFO_* in ra_defines.h)
 * \param <id_ascii> ASCII-id of the info (needed for some interfaces)
 * \param <name> name of the info
 * \param <desc> description of the info
 * \param <type> type of the info-value
 */
struct meta_info
{
	int id;
	char *ascii_id;
	char *name;
	char *desc;
	int type;
};				/* struct meta_info */

#ifdef _DEFINE_INFO_STRUCTS
/* order of elements must match the order of enum ra_info_ids in ra_defines.h
   (because bsearch() is used in get_meta_info()) */
static struct meta_info _meta[] = {
	{RA_INFO_NUM_PLUGINS_L, "num_plugins", gettext_noop("#plugins"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_VERSION_C, "lib_version", gettext_noop("libRASCH version"),
	 gettext_noop("version of libRASCH"), RA_VALUE_TYPE_CHAR},


	{RA_INFO_NUM_SESSIONS_L, "num_sessions", gettext_noop("#sessions"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_NUM_OBJ_INFOS_L, "num_obj_infos", gettext_noop("#obj. infos"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_NUM_REC_GEN_INFOS_L, "num_rec_gen_infos", gettext_noop("#general rec. infos"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_NUM_REC_DEV_INFOS_L, "num_rec_dev_infos", gettext_noop("#device infos"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_NUM_REC_CH_INFOS_L, "num_rec_ch_infos", gettext_noop("#ch. infos"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_NUM_EVAL_INFOS_L, "num_eval_infos", gettext_noop("#evaluation infos"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_MAX_SAMPLERATE_D, "max_samplerate", gettext_noop("max. samplerate"),
	 gettext_noop("maximum samplrate used in measurement"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_CH_XSCALE_D, "ch_xscale", gettext_noop("x-axis scale"),
	 gettext_noop("factor for a channel to scale positions"), RA_VALUE_TYPE_DOUBLE}, 

	{RA_INFO_SIZE_L, "meas_size", gettext_noop("size [Bytes]"),
	 gettext_noop("size needed on disc of measurement in Bytes"), RA_VALUE_TYPE_LONG},

	{RA_INFO_FILES_CA, "meas_files", gettext_noop("files"),
	 gettext_noop("files belonging to measurement"), RA_VALUE_TYPE_CHAR_ARRAY},

	{RA_INFO_PATH_C, "meas_path", gettext_noop("path"),
	 gettext_noop("full path of measurement incl. measurement name"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_DIR_L,	"meas_in_dir", gettext_noop("data in dir"),
	 gettext_noop("flag if measurement was saved in directory"), RA_VALUE_TYPE_LONG},


	{RA_INFO_OBJ_PERSON_NAME_C, "person_name", gettext_noop("Name"),
	 gettext_noop("name of the person measured"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_FORENAME_C, "person_forename", gettext_noop("Forename"),
	 gettext_noop("forename of the person measured"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_BIRTHDAY_C, "person_birthday", gettext_noop("Birthday"),
	 gettext_noop("birthday of the person measured"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_GENDER_C, "person_gender", gettext_noop("gender"),
	 gettext_noop("gender of the person measured"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_AGE_L, "person_age", gettext_noop("Age"),
	 gettext_noop("age of the person measured"), RA_VALUE_TYPE_LONG},

	{RA_INFO_OBJ_PERSON_HEIGHT_L, "person_height", gettext_noop("Height"),
	 gettext_noop("height of the person measured in cm"), RA_VALUE_TYPE_LONG},

	{RA_INFO_OBJ_PERSON_WEIGHT_L, "person_weight", gettext_noop("Weight"),
	 gettext_noop("weight of the person measured in kg"), RA_VALUE_TYPE_LONG},

	{RA_INFO_OBJ_PERSON_STREET_C, "person_street", gettext_noop("street"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_CITY_C, "person_city", gettext_noop("city"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_POSTALCODE_C, "person_postalcode", gettext_noop("postal code"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_COUNTRY_C, "person_country", gettext_noop("country"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_ADDRESS_C, "person_address", gettext_noop("address"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_PHONE1_C, "person_phone1", gettext_noop("phone1"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_PHONE2_C, "person_phone2", gettext_noop("phone2"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_PHONE_MOBILE_C, "person_phone_mobile", gettext_noop("mobile_phone"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_FAX_C, "person_fax", gettext_noop("fax"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_EMAIL_C, "person_email", gettext_noop("email"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_WEBSITE_C, "person_website", gettext_noop("website"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PERSON_COMMENT_C, "person_comment", gettext_noop("comment"),
	 "", RA_VALUE_TYPE_CHAR},


	{RA_INFO_OBJ_PATIENT_ID_C, "patient_id", gettext_noop("ID"),
	 gettext_noop("Patient-ID"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_REASON_C, "patient_reason", gettext_noop("reason"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_DIAGNOSIS_C, "patient_diagnosis", gettext_noop("diagnosis"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_THERAPY_C, "patient_therapy", gettext_noop("therapy"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_MEDICATION_C, "patient_medication", gettext_noop("medication"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_HOSPITAL_C, "patient_hospital", gettext_noop("hospital"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_DEPARTMENT_C, "patient_department", gettext_noop("department"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_DOCTOR_C, "patient_doctor", gettext_noop("doctor"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_OBJ_PATIENT_EXAMINER_C, "patient_examiner", gettext_noop("examiner"),
	 "", RA_VALUE_TYPE_CHAR},


	{RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L, "num_fetus", gettext_noop("number of fetus"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C, "gestation_date", gettext_noop("gestation date"),
	 "", RA_VALUE_TYPE_CHAR},


	{RA_INFO_REC_GEN_TYPE_L, "rec_type", gettext_noop("rec.type"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_NAME_C, "rec_name", gettext_noop("name"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_DESC_C, "rec_desc", gettext_noop("description"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_COMMENT_C, "rec_comment", gettext_noop("comment"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_TIME_C, "rec_time", gettext_noop("rec.time"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_DATE_C, "rec_date", gettext_noop("rec.date"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_DURATION_SEC_D, "rec_duration_sec", gettext_noop("rec.duration[sec]"),
	 "", RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_GEN_NUMBER_L, "rec_number", gettext_noop("rec.number"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_NUM_SUB_REC_L, "rec_num_sub_rec", gettext_noop("#sub-rec's"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_NUM_DEVICES_L, "rec_num_devices", gettext_noop("#device's"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_NUM_CHANNEL_L, "rec_num_channel", gettext_noop("#channels"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_OFFSET_L, "rec_offset", gettext_noop("offset[msec]"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_PATH_C, "rec_path", gettext_noop("full path"),
	 gettext_noop("full path of recording incl. recording name"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_GEN_DIR_L, "rec_is_directory", gettext_noop("rec. in dir"),
	 gettext_noop("flag if recording is saved in directory"), RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_GEN_MM_SEC_D, "rec_mm_per_sec", gettext_noop("mm/sec"),
	 gettext_noop("mm per second (hint for displaing signal)"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_GEN_IS_SAMPLING_L, "rec_is_running", gettext_noop("rec is running"),
	 gettext_noop("flag if device is recording (start/stop it with ra_info_set())"), RA_VALUE_TYPE_LONG},


	{RA_INFO_REC_DEV_HW_NAME_C, "dev_hw_name", gettext_noop("dev-hw-name"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_HW_MANUFACTURER_C, "dev_hw_manufacturer", gettext_noop("dev-hw-manufacturer"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_HW_SERIAL_NO_C, "dev_hw_serial_no", gettext_noop("dev-hw-serial#"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_HW_VERSION_C, "dev_hw_version", gettext_noop("dev-hw-version"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_SW_NAME_C, "dev_sw_name", gettext_noop("dev-sw-name"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_SW_MANUFACTURER_C, "dev_sw_manufacturer", gettext_noop("dev-sw-manufacturer"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_SW_SERIAL_NO_C, "dev_sw_serial_no", gettext_noop("dev-sw-serial#"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_DEV_SW_VERSION_C, "dev_sw_version", gettext_noop("dev-sw-version"),
	 "", RA_VALUE_TYPE_CHAR},


	{RA_INFO_REC_CH_NAME_C, "ch_name", gettext_noop("ch-name"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_CH_DESC_C, "ch_desc", gettext_noop("ch-desc"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_CH_NUM_SAMPLES_L, "ch_num_samples", gettext_noop("ch-#samples"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_START_SAMPLE_L, "ch_start_sample", gettext_noop("ch-first-sample-avail"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_LAST_SAMPLE_L, "ch_last_sample", gettext_noop("ch-last-sample-avail"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_SAMPLERATE_D, "ch_samplerate", gettext_noop("ch-samplerate"),
	 "", RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_NUM_BITS_L, "ch_num_bits", gettext_noop("ch-#bits"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_AMP_RESOLUTION_D, "ch_amp_res", gettext_noop("ch-resolution"),
	 "", RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_SAMPLE_TYPE_L, "ch_sample_type", gettext_noop("ch-sample-type"),
	 gettext_noop("storage type of the sample values (e.g. integer, real)"), RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_UNIT_C, "ch_unit", gettext_noop("ch-unit"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_REC_CH_CENTER_VALUE_D, "ch_center_value", gettext_noop("center value"),
	 gettext_noop("value[unit] signal is centered"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_CENTER_SAMPLE_D, "ch_center_sample", gettext_noop("center sample"),
	 gettext_noop("center value as sample-value"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_MIN_ADC_D, "ch_min_adc", gettext_noop("min.adc.value"),
	 gettext_noop("minimum value in ADC amp-levels"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_MAX_ADC_D, "ch_max_adc", gettext_noop("max.adc.value"),
	 gettext_noop("maximum value in ADC amp-levels"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_MIN_UNIT_D, "ch_min_unit", gettext_noop("min.value"),
	 gettext_noop("minimum value in units"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_MAX_UNIT_D, "ch_max_unit", gettext_noop("max.value"),
	 gettext_noop("maximum value in units"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_MM_UNIT_D, "ch_mm_per_unit", gettext_noop("mm/unit"),
	 gettext_noop("mm per unit (hint for displaing signal)"), RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_REC_CH_TYPE_L, "ch_type", gettext_noop("type"),
	 gettext_noop("type of channel (e.g. ecg, eeg)"), RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_DO_IGNORE_VALUE_L, "do_ignore_value", gettext_noop("do-ignore-value"),
	 gettext_noop("flag if some values of the channel should be ignored"), RA_VALUE_TYPE_LONG},

	{RA_INFO_REC_CH_IGNORE_VALUE_D, "ignore_value", gettext_noop("ignore-value"),
	 gettext_noop("value which should be ignored"), RA_VALUE_TYPE_DOUBLE},


	{RA_INFO_EVAL_NAME_C, "eval_name", gettext_noop("eval-name"),
	 gettext_noop("name of the evaluation"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_DESC_C, "eval_desc", gettext_noop("eval-description"),
	 gettext_noop("description of the evaluation"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_ADD_TS_C, "eval_add_timestamp", gettext_noop("eval add ts"),
	 gettext_noop("timestamp when eval was added"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_MODIFY_TS_C, "eval_modify_timestamp", gettext_noop("eval modify ts"),
	 gettext_noop("timestamp when eval was last modified"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_USER_C, "eval_user", gettext_noop("user"),
	 gettext_noop("user who added eval"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_HOST_C, "eval_host", gettext_noop("host"),
	 gettext_noop("machine on which eval was added"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_PROG_C, "eval_program", gettext_noop("program"),
	 gettext_noop("program which added eval"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_EVAL_ORIGINAL_L, "eval_is_original", gettext_noop("original-eval"),
	 gettext_noop("evaluation done in recording system"), RA_VALUE_TYPE_LONG},

	{RA_INFO_EVAL_DEFAULT_L, "eval_is_default", gettext_noop("default-eval"),
	 gettext_noop("evaluation which should be used"), RA_VALUE_TYPE_LONG},


	{RA_INFO_CLASS_ASCII_ID_C, "class_id_ascii", gettext_noop("class-id-ascii"),
	 gettext_noop("id-string of the event class"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_CLASS_NAME_C, "class_name", gettext_noop("class-name"),
	 gettext_noop("name of the event class"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_CLASS_DESC_C, "class_desc", gettext_noop("class-descritpion"),
	 gettext_noop("description of the event class"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_CLASS_EV_NUM_L, "class_num_events", gettext_noop("#events"),
	 gettext_noop("number of events"), RA_VALUE_TYPE_LONG},


	{RA_INFO_PROP_ASCII_ID_C, "prop_id_ascii", gettext_noop("prop-id-ascii"),
	 gettext_noop("id-string of event property"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PROP_VALTYPE_L, "prop_value_type", gettext_noop("prop-value type"),
	 gettext_noop("type of the event-property values"), RA_VALUE_TYPE_LONG},

	{RA_INFO_PROP_NAME_C, "prop_name", gettext_noop("prop-name"),
	 gettext_noop("name of event property"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PROP_DESC_C, "prop_desc", gettext_noop("prop-desc"),
	 gettext_noop("description of event property"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PROP_UNIT_C, "prop_unit", gettext_noop("unit"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PROP_HAS_MINMAX_L, "prop_has_minmax", gettext_noop("have min/max-values"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_PROP_MIN_D, "prop_min", gettext_noop("min-value"),
	 "", RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_PROP_MAX_D, "prop_max", gettext_noop("max-value"),
	 "", RA_VALUE_TYPE_DOUBLE},

	{RA_INFO_PROP_IGNORE_VALUE_D, "prop_ignore_value", gettext_noop("ignore-value"),
	 gettext_noop("value which should be ignored (if not available, an invalid value-handle will be returned)"),
	 RA_VALUE_TYPE_DOUBLE},


	{RA_INFO_SUM_ASCII_ID_C, "sum_id_ascii", gettext_noop("summary-id-ascii"),
	 gettext_noop("id-string of event summary"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_SUM_NAME_C, "sum_name", gettext_noop("summary name"),
	 gettext_noop("name of the event summary"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_SUM_DESC_C, "sum_desc", gettext_noop("summary descritption"),
	 gettext_noop("description of the event summary"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_SUM_NUM_DIM_L,	"sum_num_dim", gettext_noop("#dimensions"),
	 gettext_noop("number of dimensions used for the summary"), RA_VALUE_TYPE_LONG},

	{RA_INFO_SUM_DIM_UNIT_C, "sum_dim_unit", gettext_noop("dimension unit"),
	 gettext_noop("unit of the dimension"), RA_VALUE_TYPE_CHAR_ARRAY},

	{RA_INFO_SUM_DIM_NAME_C, "sum_dim_name", gettext_noop("dimension name"),
	 gettext_noop("name of the dimension"), RA_VALUE_TYPE_CHAR_ARRAY},

	{RA_INFO_SUM_NUM_CH_L, "sum_num_ch", gettext_noop("#summary channels"),
	 gettext_noop("number of channels where summary where got"), RA_VALUE_TYPE_LONG},

	{RA_INFO_SUM_CH_NUM_L, "sum_ch_num", gettext_noop("ch-number"),
	 gettext_noop("channel-number used"), RA_VALUE_TYPE_LONG},

	{RA_INFO_SUM_CH_FIDUCIAL_L, "sum_ch_fiducial", gettext_noop("fiducial offset"),
	 gettext_noop("offset of fiducial point (-1 if none)"), RA_VALUE_TYPE_LONG},


	{RA_INFO_PL_NAME_C, "pl_name", gettext_noop("plugin name"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_DESC_C, "pl_desc", gettext_noop("plugin description"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_FILE_C, "pl_file", gettext_noop("plugin filename"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_USE_IT_L, "pl_use_it", gettext_noop("use plugin"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_PL_TYPE_L, "pl_type", gettext_noop("plugin type"),
	 "", RA_VALUE_TYPE_LONG},

	{RA_INFO_PL_VERSION_C, "pl_version", gettext_noop("plugin version"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_BUILD_TS_C, "pl_build_ts", gettext_noop("plugin build-timestamp"),
	 "", RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_LICENSE_L, "pl_license", gettext_noop("plugin license"),
	 "", RA_VALUE_TYPE_LONG},


	{RA_INFO_PL_NUM_OPTIONS_L, "pl_num_opt", gettext_noop("#options"),
	 gettext_noop("number of options available in the plugin"), RA_VALUE_TYPE_LONG},

	{RA_INFO_PL_OPT_NAME_C,	"pl_opt_name", gettext_noop("opt-name"),
	 gettext_noop("a short name of the option"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_OPT_DESC_C,	"pl_opt_desc", gettext_noop("opt-desc"),
	 gettext_noop("a description of the option"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_OPT_TYPE_L,	"pl_opt_type", gettext_noop("opt-type"),
	 gettext_noop("type of the option (eg. long, double, char)"), RA_VALUE_TYPE_LONG},


	{RA_INFO_PL_NUM_RESULTS_L, "pl_num_results", gettext_noop("#results"),
	 gettext_noop("number of results in the plugin"), RA_VALUE_TYPE_LONG},

	{RA_INFO_PL_RES_NAME_C, "res_name", gettext_noop("res-name"),
	 gettext_noop("a short name of the result"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_RES_DESC_C, "res_desc", gettext_noop("res-desc"),
	 gettext_noop("a description of the result"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_PL_RES_TYPE_L, "res_type", gettext_noop("res-type"),
	 gettext_noop("type of the result (eg. long, double, char)"), RA_VALUE_TYPE_LONG},

	{RA_INFO_PL_RES_DEFAULT_L, "res_default", gettext_noop("res-default"),
	 gettext_noop("flag if the result belongs to the default results"), RA_VALUE_TYPE_LONG},


	{RA_INFO_SES_NAME_C, "session_name", gettext_noop("session-name"),
	 gettext_noop("name of the session"), RA_VALUE_TYPE_CHAR},

	{RA_INFO_SES_DESC_C, "session_desc", gettext_noop("session-desc"),
	 gettext_noop("a description of the session"), RA_VALUE_TYPE_CHAR},


	{RA_INFO_PROC_NUM_RES_SETS_L, "proc_num_res_sets", gettext_noop("#result sets"),
	 gettext_noop("number of available result sets"), RA_VALUE_TYPE_LONG},

	{RA_INFO_PROC_NUM_RES_L, "proc_num_res", gettext_noop("#results"),
	 gettext_noop("number of available results per set"), RA_VALUE_TYPE_LONG},
};  /* struct meta_info _meta[] */
#endif /* _DEFINE_INFO_STRUCTS */


/* If memory is shared between modules (plugins) use this functions. Under
   Windows it seems, that malloc use the DLL stack and other DLL's can't access
   this memory (I thought that DLL's use the same address space as the "parent"
   process, but it seems I'm wrong; under Linux it works as expected :-) ) */
#ifdef WIN32
#include <windows.h>
#define ra_alloc_mem(x)      GlobalAlloc(GMEM_FIXED, x)
#define ra_realloc_mem(p,x)  (p ? GlobalReAlloc(p, x, 0) : ra_alloc_mem(x))
#define ra_free_mem(x)       GlobalFree(x)
#else
#define ra_alloc_mem(x)      malloc(x)
#define ra_realloc_mem(p,x)  realloc(p, x)
#define ra_free_mem(x)       free(x)
#endif


/* get info about minfo */
struct meta_info *get_meta_info(int id);
int find_ra_info_by_name(value_handle vh, const char *c);
int get_ra_info_id_ascii_by_id(long id, char *id_ascii);

/* some helper functions needed in modules */
void split_filename(const char *file, char sep, char *dir, char *name, char *ext);
void split_url(const char *url, char *protocol, char *server, char *file);
void set_one_session(struct ra_meas *meas, const char *name, const char *desc);
void free_session_rec(struct ra_rec *rh);
int fill_predef_class_info(long id, value_handle vh_id, value_handle vh_name, value_handle vh_desc);
int fill_predef_class_info_ascii(const char *ascii_id, value_handle vh_name, value_handle vh_desc);
long get_class_id(const char *ascii_id);
int fill_predef_prop_info(int prop, value_handle vh_id, value_handle vh_type, value_handle vh_len,
			  value_handle vh_name, value_handle vh_desc,  value_handle vh_unit, value_handle vh_use_minmax,
			  value_handle vh_min, value_handle vh_max, value_handle has_ign_val, value_handle vh_ign_val);
int fill_predef_prop_info_ascii(const char *ascii_id, value_handle vh_type, value_handle vh_len,
				value_handle vh_name, value_handle vh_desc,  value_handle vh_unit,
				value_handle vh_use_minmax, value_handle vh_min, value_handle vh_max,
				value_handle has_ign_val, value_handle vh_ign_val);
long get_prop_id(const char *ascii_id);
int get_intersection_long(const long *v1, long num1, const long *v2, long num2,
						  long **idx_v2_in_v1, long *num_idx);

#ifdef __cplusplus
}
#endif

#endif /* PL_GENERAL_H */
