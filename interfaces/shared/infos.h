#ifndef INFOS_H
#define INFOS_H

struct info_map
{
    const char *name;
    long id;
};  /* struct info_map */


struct info_map infos[] = 
{
	/* ---------- library infos #ra_lib_get_info# ---------- */
	{ "num_plugins", RA_INFO_NUM_PLUGINS_L }, /* number of plugins */
	{ "lib_version", RA_INFO_VERSION_C }, /* version of libRASCH */

	/* ---------- measurement infos #ra_meas_get_info# ---------- */
	{ "num_sessions", RA_INFO_NUM_SESSIONS_L },  /* number of sessions in a measurement */
	{ "num_obj_infos", RA_INFO_NUM_OBJ_INFOS_L },  /* number of infos about a measurment object */
	{ "num_rec_gen_infos", RA_INFO_NUM_REC_GEN_INFOS_L },  /* number of general infos */
	{ "num_rec_dev_infos", RA_INFO_NUM_REC_DEV_INFOS_L },  /* number of infos about a recording device */
	{ "num_rec_ch_infos", RA_INFO_NUM_REC_CH_INFOS_L },  /* number of infos about a channel */
	{ "num_eval_infos", RA_INFO_NUM_EVAL_INFOS_L },  /* number of infos about an evaluation */
	{ "max_samplerate", RA_INFO_MAX_SAMPLERATE_D }, /* maximum samplerate used in the recording */
	{ "ch_xscale", RA_INFO_CH_XSCALE_D }, /* position scale-factor for a channel */
	{ "meas_size", RA_INFO_SIZE_L }, /* size needed on disc of measurement in Bytes */
	{ "meas_file", RA_INFO_FILES_CA }, /* files belonging to measurement */
	{ "meas_path", RA_INFO_PATH_C },  /* full path of measurement incl. measurement name */
	{ "meas_in_dir", RA_INFO_DIR_L }, /* flag if measurement was saved in directory */
	
	/* ---------- measurement-object (person) infos #ra_obj_get_info# ---------- */
	{ "person_name", RA_INFO_OBJ_PERSON_NAME_C },  /* last name of the measurement object */
	{ "person_forename", RA_INFO_OBJ_PERSON_FORENAME_C },  /* first name of the measurement object */
	{ "person_birthday", RA_INFO_OBJ_PERSON_BIRTHDAY_C },  /* birthday of the measurement object */
	{ "person_gender", RA_INFO_OBJ_PERSON_GENDER_C },  /* gender  of the measurement object*/
	{ "person_age", RA_INFO_OBJ_PERSON_AGE_L },  /* age  of the measurement object*/
	{ "person_height", RA_INFO_OBJ_PERSON_HEIGHT_L },  /* height of the measurement object */
	{ "person_weight", RA_INFO_OBJ_PERSON_WEIGHT_L },  /* weight of the measurement object */
	{ "person_street", RA_INFO_OBJ_PERSON_STREET_C },  /* street of the address of the measurement object */
	{ "person_city", RA_INFO_OBJ_PERSON_CITY_C },  /* city of the address the measurement object */
	{ "person_postalcode", RA_INFO_OBJ_PERSON_POSTALCODE_C },  /* postalcode of the address the measurement object */
	{ "person_country", RA_INFO_OBJ_PERSON_COUNTRY_C },  /* country of the address the measurement object */
	{ "person_address", RA_INFO_OBJ_PERSON_ADDRESS_C },  /* address the measurement object (sometimes this field contains the whole address) */
	{ "person_phone1", RA_INFO_OBJ_PERSON_PHONE1_C },  /* phone-number of the measurement object */
	{ "person_phone2", RA_INFO_OBJ_PERSON_PHONE2_C },  /* phone-number 2 of the measurement object */
	{ "person_fax", RA_INFO_OBJ_PERSON_FAX_C },  /* fax-number of the measurement object */
	{ "person_email", RA_INFO_OBJ_PERSON_EMAIL_C },  /* email-address of the measurement object */
	{ "person_website", RA_INFO_OBJ_PERSON_WEBSITE_C },  /* website of the measurement object */
	{ "person_comment", RA_INFO_OBJ_PERSON_COMMENT_C },  /* a comment about the measurement object */
	/* ---------- measurement-object (patient) infos #ra_obj_get_info# ---------- */
	/* (for person related infos use RA_INFO_OBJ_PERSON_*) */
	{ "patient_id", RA_INFO_OBJ_PATIENT_ID_C },  /* ID of the patient */
	{ "patient_reason", RA_INFO_OBJ_PATIENT_REASON_C },  /* reasonf of the recording */
	{ "patient_diagnosis", RA_INFO_OBJ_PATIENT_DIAGNOSIS_C },  /* diagnosis */
	{ "patient_therapy", RA_INFO_OBJ_PATIENT_THERAPY_C },  /* therapy */
	{ "patient_medication", RA_INFO_OBJ_PATIENT_MEDICATION_C },  /* medication of the patient */
	{ "patient_hospital", RA_INFO_OBJ_PATIENT_HOSPITAL_C },  /* Hospital where the recording was performed */
	{ "patient_department", RA_INFO_OBJ_PATIENT_DEPARTMENT_C },  /* Department where the recording was performed */
	{ "patient_doctor", RA_INFO_OBJ_PATIENT_DOCTOR_C },  /* Doctor who analyze the evaluation of the recording */
	{ "patient_examiner", RA_INFO_OBJ_PATIENT_EXAMINER_C },  /* Person who evaluate the recording */
	/* ---------- pregnant woman ---------- */
	{ "preg_num_fetus", RA_INFO_OBJ_PREG_WOMAN_NUM_FETUS_L },  /* number fetus */
	{ "preg_gestation_date", RA_INFO_OBJ_PREG_WOMAN_GESTATION_DATE_C },  /* gestation date */
	
	/* ---------- recording infos #ra_rec_get_info# ---------- */
	{ "rec_type", RA_INFO_REC_GEN_TYPE_L },	/* type of recording (see RA_REC_TYPE_*) */
	{ "rec_name", RA_INFO_REC_GEN_NAME_C },	/* name of recording */
	{ "rec_desc", RA_INFO_REC_GEN_DESC_C },  /* description of the recording */
	{ "rec_comment", RA_INFO_REC_GEN_COMMENT_C },  /* comment about the recording */
	{ "rec_time", RA_INFO_REC_GEN_TIME_C },	/* start time of recording (hh:mm:ss) */
	{ "rec_date", RA_INFO_REC_GEN_DATE_C },	/* start date of recording (dd.mm.yyyy) */
	{ "rec_duration_sec", RA_INFO_REC_GEN_DURATION_SEC_D },  /* duration of the recording in seconds */
	{ "rec_number", RA_INFO_REC_GEN_NUMBER_L },	/* number of recording (0: main recording) */
	{ "rec_num_sub_rec", RA_INFO_REC_GEN_NUM_SUB_REC_L },	/* number of sub-recordings */
	{ "rec_num_devices", RA_INFO_REC_GEN_NUM_DEVICES_L },  /* number of devices used for recording */
	{ "rec_num_channel", RA_INFO_REC_GEN_NUM_CHANNEL_L },	/* number of channels */
	{ "rec_offset", RA_INFO_REC_GEN_OFFSET_L },	/* if sub-recording: num of msec time shift between
							   the sub-recordings (first sub-rec. is 0) */
	{ "rec_path", RA_INFO_REC_GEN_PATH_C },  /* full path of recording incl. recording name */
	{ "rec_is_directory", RA_INFO_REC_GEN_DIR_L },   /* flag if recording was saved in directory */
	{ "rec_mm_per_sec", RA_INFO_REC_GEN_MM_SEC_D },    /* mm per second (hint for displaing signal) */
	/* ---------- device infos #ra_dev_get_info# ---------- */
	{ "dev_hw_name", RA_INFO_REC_DEV_HW_NAME_C },  /* name of the recording device */
	{ "dev_hw_manufacturer", RA_INFO_REC_DEV_HW_MANUFACTURER_C },  /* manufacturer of the recording device */
	{ "dev_hw_serial_no", RA_INFO_REC_DEV_HW_SERIAL_NO_C },  /* serial-number of the recording device */
	{ "dev_hw_version", RA_INFO_REC_DEV_HW_VERSION_C },  /* hardware-version of the recording device */
	{ "dev_sw_name", RA_INFO_REC_DEV_SW_NAME_C },  /* software-name of the recording device */
	{ "dev_sw_manufacturer", RA_INFO_REC_DEV_SW_MANUFACTURER_C },  /* manufturer of the software of the recording device */
	{ "dev_sw_serial_no", RA_INFO_REC_DEV_SW_SERIAL_NO_C },  /* serial-number of the software of the recording device */
	{ "dev_sw_version", RA_INFO_REC_DEV_SW_VERSION_C },  /* version of the software of the recording device */
	/* ---------- channel infos #ra_ch_get_info# ---------- */
	{ "ch_name", RA_INFO_REC_CH_NAME_C },  /* name of the channel */
	{ "ch_desc", RA_INFO_REC_CH_DESC_C },  /* description of the channel */
	{ "ch_num_samples", RA_INFO_REC_CH_NUM_SAMPLES_L },  /* number of samples recorded in the channel */
	{ "ch_samplerate", RA_INFO_REC_CH_SAMPLERATE_D },  /* samplerate used in the channel */
	{ "ch_num_bits", RA_INFO_REC_CH_NUM_BITS_L },  /* number of bits used for one sample */
	{ "ch_amp_res", RA_INFO_REC_CH_AMP_RESOLUTION_D },  /* value in 'bits' representing one unit */
	{ "ch_unit", RA_INFO_REC_CH_UNIT_C },  /* unit of the channel */
	{ "ch_center_value", RA_INFO_REC_CH_CENTER_VALUE_D },  /* value[unit] signal is centered */
	{ "ch_center_sample", RA_INFO_REC_CH_CENTER_SAMPLE_D }, /* center value as sample-value */
	{ "ch_min_adc", RA_INFO_REC_CH_MIN_ADC_D }, /* minimum value in ADC amp-levels */
	{ "ch_max_adc", RA_INFO_REC_CH_MAX_ADC_D }, /* minimum value in ADC amp-levels */
	{ "ch_min_unit", RA_INFO_REC_CH_MIN_UNIT_D },  /* minimum value in units */
	{ "ch_max_unit", RA_INFO_REC_CH_MAX_UNIT_D },  /* maximum value in units */
	{ "ch_mm_per_unit", RA_INFO_REC_CH_MM_UNIT_D },   /* mm per unit (hint for displaing signal) */
	{ "ch_type", RA_INFO_REC_CH_TYPE_L },  /* type of channel (see above RA_CH_TYPE_*) */
	
	/* ---------- evaluation infos #ra_eval_get_info# ---------- */
	{ "eval_name", RA_INFO_EVAL_NAME_C },  /* name of evaluation (if any) */
	{ "eval_desc", RA_INFO_EVAL_DESC_C },  /* comment of evaluation */
	{ "eval_add_timestamp", RA_INFO_EVAL_ADD_TS_C },  /* timestamp when eval was added */
	{ "eval_modify_timestamp", RA_INFO_EVAL_MODIFY_TS_C },  /* timestamp when eval was last modified */
	{ "eval_user", RA_INFO_EVAL_USER_C },  /* user who added eval */
	{ "eval_host", RA_INFO_EVAL_HOST_C },  /* machine on which eval was added */
	{ "eval_program", RA_INFO_EVAL_PROG_C },  /* program which added eval */
	{ "eval_is_original", RA_INFO_EVAL_ORIGINAL_L }, /* evaluation done in recording system */
	{ "eval_is_default", RA_INFO_EVAL_DEFAULT_L }, /* evaluation which should be used */
	/* ----------evaluation-set infos #ra_evset_get_info# ---------- */
	{ "class_id_ascii", RA_INFO_CLASS_ASCII_ID_C }, /* ASCII id of event-class */
	{ "class_name", RA_INFO_CLASS_NAME_C }, /* name of event-class */
	{ "class_desc", RA_INFO_CLASS_DESC_C }, /* description of event-class */
	{ "class_num_events", RA_INFO_CLASS_EV_NUM_L },  /* number of events in event-class */
	/* ---------- event-property infos #ra_prop_get_info# ---------- */
	{ "prop_id_ascii", RA_INFO_PROP_ASCII_ID_C },  /* ASCII id of event-property, must be unique */
	{ "prop_value_type", RA_INFO_PROP_VALTYPE_L },  /* type of the event-property values (short, long, double) */
	{ "prop_name", RA_INFO_PROP_NAME_C },  /* name of event-property, must be unique */
	{ "prop_desc", RA_INFO_PROP_DESC_C },  /* description of event property */
	{ "prop_unit", RA_INFO_PROP_UNIT_C }, /* unit of the property */
	{ "prop_has_minmax", RA_INFO_PROP_HAS_MINMAX_L }, /* flag if property has min-/max-values */
	{ "prop_min", RA_INFO_PROP_MIN_D }, /* normal min-value of the property */
	{ "prop_max", RA_INFO_PROP_MAX_D }, /* normal max-value of the property */
	{ "prop_ignore_value", RA_INFO_PROP_IGNORE_VALUE_D }, /* ignore value (or undef if no one is available) */
	/* ---------- event-summary infos #not yet implemented# ---------- */
	{ "sum_id_ascii", RA_INFO_SUM_ASCII_ID_C },  /* ASCII id of event-summary, must be unique */
	{ "sum_name", RA_INFO_SUM_NAME_C },  /* name of event-summary */
	{ "sum_desc", RA_INFO_SUM_DESC_C },  /* description of event-summary */
	{ "sum_num_dim", RA_INFO_SUM_NUM_DIM_L },  /*  */
	{ "sum_dim_unit", RA_INFO_SUM_DIM_UNIT_C },  /*  */
	{ "sum_dim_name", RA_INFO_SUM_DIM_NAME_C },  /*  */
	{ "sum_num_ch", RA_INFO_SUM_NUM_CH_L },  /*  */
	{ "sum_ch_num", RA_INFO_SUM_CH_NUM_L },  /*  */
	{ "sum_ch_fiducial", RA_INFO_SUM_CH_FIDUCIAL_L },  /*  */
	/* ---------- plugin infos #ra_plugin_get_info# ---------- */
	{ "pl_name", RA_INFO_PL_NAME_C },  /* name of plugin, used for identification */
	{ "pl_desc", RA_INFO_PL_DESC_C },  /* description of plugin */
	{ "pl_file", RA_INFO_PL_FILE_C }, /* filename with path of the plugin */
	{ "pl_use_it", RA_INFO_PL_USE_IT_L }, /* flag if plugin shoul be used or not */
	{ "pl_type", RA_INFO_PL_TYPE_L }, /* type of plugin */
	{ "pl_version", RA_INFO_PL_VERSION_C }, /* version of plugin */
	{ "pl_build_ts", RA_INFO_PL_BUILD_TS_C }, /* build timestamp of plugin */
	{ "pl_license", RA_INFO_PL_LICENSE_L }, /* license used for plugin */
	{ "pl_num_opt", RA_INFO_PL_NUM_OPTIONS_L },  /* number of options in the plugin */
	{ "pl_num_results", RA_INFO_PL_NUM_RESULTS_L },  /* number of values returned from plugin */
	/* ---------- plugin-option infos #ra_plugin_get_option_info# ---------- */
	{ "pl_opt_name", RA_INFO_PL_OPT_NAME_C },  /* short name of option */
	{ "pl_opt_desc", RA_INFO_PL_OPT_DESC_C },  /* description of option */
	{ "pl_opt_type", RA_INFO_PL_OPT_TYPE_L },  /* type of option */
	/* ---------- plugin-results infos #ra_proc_get_result_info# ---------- */
	{ "proc_num_res_sets", RA_INFO_PROC_NUM_RES_SETS_L }, /* number of available result sets */
	{ "proc_num_res", RA_INFO_PROC_NUM_RES_L }, /* number of available results per set */
	{ "res_name", RA_INFO_PL_RES_NAME_C },  /* short name of result (can be used as table header) */
	{ "res_desc", RA_INFO_PL_RES_DESC_C },  /* description of result */
	{ "res_default", RA_INFO_PL_RES_DEFAULT_L },  /* flag if result belongs to default-values (=1) */
	/* ----------  ---------- */
	{ "session_name", RA_INFO_SES_NAME_C },  /* name of the session */
	{ "session_desc", RA_INFO_SES_DESC_C }, /* a description of the session */
};  /* infos */


#endif  /* INFOS_H */

