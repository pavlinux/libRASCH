/**
 * \file ra.h
 *
 * The header file contains the prototypes of most of the API functions.
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

#ifndef _RA_H
#define _RA_H

#include <stddef.h>

#include <ra_defines.h>
#include <ra_error.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------ interface ------------------------------ */
LIBRAAPI ra_handle ra_lib_init(void);
LIBRAAPI void ra_lib_close(ra_handle h);

LIBRAAPI long ra_lib_get_error(ra_handle h, char *text, size_t len);

LIBRAAPI ra_handle ra_lib_handle_from_any_handle(meas_handle mh);
LIBRAAPI meas_handle ra_meas_handle_from_any_handle(any_handle rh);

LIBRAAPI int ra_lib_use_plugin(ra_handle h, int plugin_index, int use_it);

LIBRAAPI plugin_handle ra_plugin_get_by_num(ra_handle h, int plugin_index, int search_all);
LIBRAAPI plugin_handle ra_plugin_get_by_name(ra_handle h, const char *name, int search_all);

LIBRAAPI ra_find_handle ra_meas_find_first(ra_handle h, const char *dir, struct ra_find_struct *rfs);
LIBRAAPI int ra_meas_find_next(ra_find_handle h, struct ra_find_struct *mfs);
LIBRAAPI void ra_meas_close_find(ra_find_handle h);

 /* fast != 0: don't read evaluation data */
LIBRAAPI meas_handle ra_meas_open(ra_handle h, const char *file, const char *eval_file, int fast);
/* creates a new measurement using the rasch-fileformat */
LIBRAAPI meas_handle ra_meas_new(ra_handle h, const char *dir, const char *name);
LIBRAAPI int ra_meas_save(meas_handle mh);
LIBRAAPI void ra_meas_close(meas_handle mh);

LIBRAAPI int ra_info_get(any_handle h, int id, value_handle vh);
LIBRAAPI int ra_info_get_by_name(any_handle h, const char *name, value_handle vh);
LIBRAAPI int ra_info_get_by_idx(any_handle h, int info_type, int idx, value_handle vh);
LIBRAAPI int ra_info_set(any_handle h, int id, value_handle vh);
LIBRAAPI int ra_info_set_by_name(any_handle h, const char *name, value_handle vh);

LIBRAAPI rec_handle ra_rec_get_first(meas_handle mh, long session);
LIBRAAPI rec_handle ra_rec_get_next(rec_handle rh);
LIBRAAPI rec_handle ra_rec_get_first_child(rec_handle rh);

/*
LIBRAAPI rec_handle ra_rec_add(meas_handle mh, rec_handle parent);
LIBRAAPI long ra_device_add(rec_handle rh);
LIBRAAPI long ra_channel_add(rec_handle rh, double samplerate, long sample_type, long compression_scheme);
LIBRAAPI long ra_session_start(rec_handle rh);
LIBRAAPI int ra_session_stop(rec_handle rh);

LIBRAAPI size_t ra_raw_write(rec_handle rh, unsigned int ch, size_t num_data, DWORD *data, DWORD *data_high);
LIBRAAPI size_t ra_raw_write_float(rec_handle rh, unsigned int ch, size_t num_data, double *data);
*/

LIBRAAPI size_t ra_raw_get(rec_handle rh, unsigned int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
LIBRAAPI size_t ra_raw_get_unit(rec_handle rh, unsigned int ch, size_t start, size_t num_data, double *data);

LIBRAAPI int ra_raw_process(long task, value_handle task_data, size_t num_data, DWORD *data, DWORD *data_high,
			    rec_handle rh, unsigned int ch);
LIBRAAPI int ra_raw_process_unit(long task, value_handle task_data, size_t num_data, double *data,
			    rec_handle rh, unsigned int ch);

LIBRAAPI int ra_gui_call(any_handle h, plugin_handle pl);

LIBRAAPI proc_handle ra_proc_get(meas_handle mh, plugin_handle pl, void (*callback) (const char *, int ));
LIBRAAPI void ra_proc_free(proc_handle proc);
LIBRAAPI int ra_proc_do(proc_handle proc);
LIBRAAPI long ra_proc_get_result_idx(proc_handle proc, const char *res_ascii_id);
LIBRAAPI int ra_proc_get_result(proc_handle proc, long res_num, long res_set, value_handle vh);

LIBRAAPI view_handle ra_view_get(meas_handle mh, plugin_handle pl, void *parent);
LIBRAAPI void ra_view_free(view_handle vih);
LIBRAAPI int ra_view_create(view_handle vih);
LIBRAAPI void *ra_view_get_window(view_handle vih, int win_number);


/* ---------- ra_value functions ---------- */
LIBRAAPI value_handle ra_value_malloc(void);
LIBRAAPI void ra_value_free(value_handle vh);

LIBRAAPI void ra_value_reset(value_handle vh);

LIBRAAPI long ra_value_get_type(value_handle vh);
LIBRAAPI int ra_value_is_ok(value_handle vh);
LIBRAAPI unsigned long ra_value_get_num_elem(value_handle vh);
LIBRAAPI long ra_value_get_info(value_handle vh);
LIBRAAPI int ra_value_set_info(value_handle vh, long id);
LIBRAAPI const char *ra_value_get_name(value_handle vh);
LIBRAAPI const char *ra_value_get_desc(value_handle vh);
LIBRAAPI int ra_value_info_modifiable(value_handle vh);
LIBRAAPI void ra_value_info_set_modifiable(value_handle vh, int can_be_modified);

LIBRAAPI int ra_value_set_number(value_handle vh, long number);
LIBRAAPI long ra_value_get_number(value_handle vh);

LIBRAAPI void ra_value_set_short(value_handle vh, const short value);
LIBRAAPI void ra_value_set_long(value_handle vh, const long value);
LIBRAAPI void ra_value_set_double(value_handle vh, const double value);
LIBRAAPI void ra_value_set_string(value_handle vh, const char *value);
LIBRAAPI void ra_value_set_string_utf8(value_handle vh, const char *value);
LIBRAAPI void ra_value_set_voidp(value_handle vh, const void *value);
LIBRAAPI void ra_value_set_short_array(value_handle vh, const short *array, unsigned long num);
LIBRAAPI void ra_value_set_long_array(value_handle vh, const long *array, unsigned long num);
LIBRAAPI void ra_value_set_double_array(value_handle vh, const double *array, unsigned long num);
LIBRAAPI void ra_value_set_string_array(value_handle vh, const char **array, unsigned long num);
LIBRAAPI void ra_value_set_string_array_utf8(value_handle vh, const char **array, unsigned long num);
LIBRAAPI void ra_value_set_voidp_array(value_handle vh, const void **array, unsigned long num);
LIBRAAPI void ra_value_set_vh_array(value_handle vh, const value_handle *array, unsigned long num);

LIBRAAPI short ra_value_get_short(value_handle vh);
LIBRAAPI long ra_value_get_long(value_handle vh);
LIBRAAPI double ra_value_get_double(value_handle vh);
LIBRAAPI const char *ra_value_get_string(value_handle vh);
LIBRAAPI const char *ra_value_get_string_utf8(value_handle vh);
LIBRAAPI const void *ra_value_get_voidp(value_handle vh);
LIBRAAPI const short *ra_value_get_short_array(value_handle vh);
LIBRAAPI const long *ra_value_get_long_array(value_handle vh);
LIBRAAPI const double *ra_value_get_double_array(value_handle vh);
LIBRAAPI const char **ra_value_get_string_array(value_handle vh);
LIBRAAPI const char **ra_value_get_string_array_utf8(value_handle vh);
LIBRAAPI const void **ra_value_get_voidp_array(value_handle vh);
LIBRAAPI const value_handle *ra_value_get_vh_array(value_handle vh);

LIBRAAPI int ra_value_copy(value_handle dest, value_handle src);
LIBRAAPI int ra_value_get_single_elem(value_handle dest, value_handle src, size_t elem_num);

/* ---------- ra_options functions ---------- */
/* Up to now only support for getting/setting options in plugins
   TODO: think about support for options for whole library */
LIBRAAPI int ra_lib_get_option(any_handle h, const char *opt_name, value_handle vh);
LIBRAAPI int ra_lib_set_option(any_handle h, const char *opt_name, value_handle vh);


/* ---------- file functions ---------- */
LIBRAAPI int ra_meas_copy(meas_handle mh, const char *dest_dir);
LIBRAAPI meas_handle ra_meas_move(meas_handle mh, const char *dest_dir);
LIBRAAPI int ra_meas_delete(meas_handle mh);


/* -------------------- implementation in eval.c -------------------- */
/* LIBRAAPI int ra_eval_load(meas_handle mh, const char *file); */
LIBRAAPI int ra_eval_save(meas_handle mh, const char *file, int use_ascii);

LIBRAAPI int ra_eval_edit_start(any_handle h);
LIBRAAPI int ra_eval_edit_complete(any_handle h);
LIBRAAPI int ra_eval_edit_cancel(any_handle h);
/* ---------- attribute functions ---------- */
LIBRAAPI int ra_eval_attribute_list(any_handle h, value_handle vh);
LIBRAAPI int ra_eval_attribute_get(any_handle h, const char *name, value_handle vh);
LIBRAAPI int ra_eval_attribute_set(any_handle h, const char *name, value_handle vh);
LIBRAAPI int ra_eval_attribute_unset(any_handle h, const char *id);
/* ---------- evaluation functions ---------- */
LIBRAAPI eval_handle ra_eval_add(meas_handle mh, const char *name, const char *desc, int original);
LIBRAAPI eval_handle ra_eval_copy(eval_handle eh, const char *name, const char *desc);
LIBRAAPI int ra_eval_delete(eval_handle eh);
LIBRAAPI int ra_eval_get_all(meas_handle mh, value_handle vh);
LIBRAAPI eval_handle ra_eval_get_original(meas_handle mh);
LIBRAAPI eval_handle ra_eval_get_default(meas_handle mh);
LIBRAAPI int ra_eval_set_default(eval_handle eh);
LIBRAAPI eval_handle ra_eval_get_handle(class_handle clh);
/* ---------- event-class functions ---------- */
LIBRAAPI class_handle ra_class_add(eval_handle eh, const char *id, const char *name, const char *desc);
LIBRAAPI class_handle ra_class_add_predef(eval_handle eh, const char *id);
LIBRAAPI int ra_class_delete(class_handle clh);
LIBRAAPI int ra_class_get(eval_handle eh, const char *id, value_handle vh);
LIBRAAPI long ra_class_add_event(class_handle clh, long start, long end);
LIBRAAPI int ra_class_add_event_mass(class_handle clh, unsigned long num_events, const long *start, const long *end, long *ev_ids);
LIBRAAPI int ra_class_del_event(class_handle clh, long event_id);
LIBRAAPI int ra_class_get_event_pos(class_handle clh, long event_id, long *start, long *end);
LIBRAAPI int ra_class_set_event_pos(class_handle clh, long event_id, long start, long end);
LIBRAAPI int ra_class_get_events(class_handle clh, long start, long end, int complete, int sort, value_handle vh);
LIBRAAPI long ra_class_get_prev_event(class_handle clh, long event_id);
LIBRAAPI long ra_class_get_next_event(class_handle clh, long event_id);
LIBRAAPI class_handle ra_class_get_handle(any_handle h);
/* ---------- event-property functions ---------- */
LIBRAAPI prop_handle ra_prop_add(class_handle clh, const char *id, long value_type, const char *name,
				 const char *desc, const char *unit, int use_minmax, double min, double max,
				 int has_ignore_value, double ignore_value);
LIBRAAPI prop_handle ra_prop_add_predef(class_handle clh, const char *id);
LIBRAAPI int ra_prop_delete(prop_handle ph);
LIBRAAPI prop_handle ra_prop_get(class_handle clh, const char *id);
LIBRAAPI int ra_prop_get_all(class_handle clh, value_handle vh);
LIBRAAPI int ra_prop_set_value(prop_handle ph, long event_id, long ch, value_handle vh);
LIBRAAPI int ra_prop_set_value_mass(prop_handle ph, const long *event_id, const long *ch, value_handle vh);
LIBRAAPI int ra_prop_get_ch(prop_handle ph, long event_id, value_handle vh);
LIBRAAPI int ra_prop_get_value(prop_handle ph, long event_id, long ch, value_handle vh);
LIBRAAPI int ra_prop_get_value_some(prop_handle ph, long *event_ids, long num_events, long ch, short *ok, value_handle vh);
LIBRAAPI int ra_prop_get_value_all(prop_handle ph, value_handle event_id, value_handle ch, value_handle value);
LIBRAAPI int ra_prop_get_events(prop_handle ph, value_handle min, value_handle max, long ch, value_handle vh);
/* ---------- event-summary functions ---------- */
LIBRAAPI sum_handle ra_sum_add(class_handle clh, const char *id, const char *name, const char *desc,
			       long num_dim, const char **dim_name, const char **dim_unit);
LIBRAAPI int ra_sum_add_ch(sum_handle sh, long ch, long fiducial_offset);
LIBRAAPI int ra_sum_delete(sum_handle sh);
LIBRAAPI int ra_sum_get(class_handle clh, const char *id, value_handle vh);
LIBRAAPI long ra_sum_add_part(sum_handle sh, long num_events, const long *events_based_on);
LIBRAAPI int ra_sum_del_part(sum_handle sh, long part_id);
LIBRAAPI int ra_sum_get_part_all(sum_handle sh, value_handle vh);
LIBRAAPI int ra_sum_set_part_data(sum_handle sh, long part_id, long ch_idx, long dim, value_handle vh);
LIBRAAPI int ra_sum_get_part_data(sum_handle sh, long part_id, long ch_idx, long dim, value_handle vh);
LIBRAAPI int ra_sum_get_part_events(sum_handle sh, long part_id, value_handle vh);
LIBRAAPI int ra_sum_set_part_events(sum_handle sh, long part_id, value_handle vh);


#ifdef __cplusplus
}
#endif

#endif /* _RA_H */
