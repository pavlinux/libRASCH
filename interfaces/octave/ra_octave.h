#ifndef RA_OCTAVE_H
#define RA_OCTAVE_H

/*
 * Don't change this file. It was created automatically and every change
 * you made will be lost after the next make.
 *
 * created using make_interfaces.pl Version 0.2.5  15.09.2008

 */


/******************** prototypes ********************/
octave_value_list if_ra_lib_init(const octave_value_list &args);
octave_value_list if_ra_lib_close(const octave_value_list &args);
octave_value_list if_ra_lib_get_error(const octave_value_list &args);
octave_value_list if_ra_lib_handle_from_any_handle(const octave_value_list &args);
octave_value_list if_ra_lib_set_option(const octave_value_list &args);
octave_value_list if_ra_lib_get_option(const octave_value_list &args);
octave_value_list if_ra_lib_info_get(const octave_value_list &args);
octave_value_list if_ra_plugin_info_get(const octave_value_list &args);
octave_value_list if_ra_result_info_get(const octave_value_list &args);
octave_value_list if_ra_meas_info_get(const octave_value_list &args);
octave_value_list if_ra_obj_info_get(const octave_value_list &args);
octave_value_list if_ra_rec_info_get(const octave_value_list &args);
octave_value_list if_ra_dev_info_get(const octave_value_list &args);
octave_value_list if_ra_ch_info_get(const octave_value_list &args);
octave_value_list if_ra_eval_info_get(const octave_value_list &args);
octave_value_list if_ra_class_info_get(const octave_value_list &args);
octave_value_list if_ra_prop_info_get(const octave_value_list &args);
octave_value_list if_ra_summary_info_get(const octave_value_list &args);
octave_value_list if_ra_sum_dim_info_get(const octave_value_list &args);
octave_value_list if_ra_sum_ch_info_get(const octave_value_list &args);
octave_value_list if_ra_info_get_by_idx(const octave_value_list &args);
octave_value_list if_ra_info_set(const octave_value_list &args);
octave_value_list if_ra_meas_handle_from_any_handle(const octave_value_list &args);
octave_value_list if_ra_meas_find(const octave_value_list &args);
octave_value_list if_ra_meas_open(const octave_value_list &args);
octave_value_list if_ra_meas_new(const octave_value_list &args);
octave_value_list if_ra_meas_save(const octave_value_list &args);
octave_value_list if_ra_meas_close(const octave_value_list &args);
octave_value_list if_ra_rec_get_first(const octave_value_list &args);
octave_value_list if_ra_rec_get_next(const octave_value_list &args);
octave_value_list if_ra_rec_get_first_child(const octave_value_list &args);
octave_value_list if_ra_raw_get(const octave_value_list &args);
octave_value_list if_ra_raw_get_unit(const octave_value_list &args);
octave_value_list if_ra_meas_copy(const octave_value_list &args);
octave_value_list if_ra_meas_move(const octave_value_list &args);
octave_value_list if_ra_meas_delete(const octave_value_list &args);
octave_value_list if_ra_lib_use_plugin(const octave_value_list &args);
octave_value_list if_ra_plugin_get_by_num(const octave_value_list &args);
octave_value_list if_ra_plugin_get_by_name(const octave_value_list &args);
octave_value_list if_ra_proc_get(const octave_value_list &args);
octave_value_list if_ra_proc_do(const octave_value_list &args);
octave_value_list if_ra_proc_get_result_idx(const octave_value_list &args);
octave_value_list if_ra_proc_get_result(const octave_value_list &args);
octave_value_list if_ra_proc_free(const octave_value_list &args);
octave_value_list if_ra_eval_save(const octave_value_list &args);
octave_value_list if_ra_eval_attribute_list(const octave_value_list &args);
octave_value_list if_ra_eval_attribute_get(const octave_value_list &args);
octave_value_list if_ra_eval_attribute_set(const octave_value_list &args);
octave_value_list if_ra_eval_attribute_unset(const octave_value_list &args);
octave_value_list if_ra_eval_add(const octave_value_list &args);
octave_value_list if_ra_eval_delete(const octave_value_list &args);
octave_value_list if_ra_eval_get_all(const octave_value_list &args);
octave_value_list if_ra_eval_get_original(const octave_value_list &args);
octave_value_list if_ra_eval_get_default(const octave_value_list &args);
octave_value_list if_ra_eval_set_default(const octave_value_list &args);
octave_value_list if_ra_eval_get_handle(const octave_value_list &args);
octave_value_list if_ra_class_add(const octave_value_list &args);
octave_value_list if_ra_class_add_predef(const octave_value_list &args);
octave_value_list if_ra_class_delete(const octave_value_list &args);
octave_value_list if_ra_class_get(const octave_value_list &args);
octave_value_list if_ra_class_add_event(const octave_value_list &args);
octave_value_list if_ra_class_del_event(const octave_value_list &args);
octave_value_list if_ra_class_get_event_pos(const octave_value_list &args);
octave_value_list if_ra_class_set_event_pos(const octave_value_list &args);
octave_value_list if_ra_class_get_events(const octave_value_list &args);
octave_value_list if_ra_class_get_prev_event(const octave_value_list &args);
octave_value_list if_ra_class_get_next_event(const octave_value_list &args);
octave_value_list if_ra_class_get_handle(const octave_value_list &args);
octave_value_list if_ra_prop_add(const octave_value_list &args);
octave_value_list if_ra_prop_add_predef(const octave_value_list &args);
octave_value_list if_ra_prop_delete(const octave_value_list &args);
octave_value_list if_ra_prop_get(const octave_value_list &args);
octave_value_list if_ra_prop_get_all(const octave_value_list &args);
octave_value_list if_ra_prop_set_value(const octave_value_list &args);
octave_value_list if_ra_prop_set_value_mass(const octave_value_list &args);
octave_value_list if_ra_prop_get_ch(const octave_value_list &args);
octave_value_list if_ra_prop_get_value(const octave_value_list &args);
octave_value_list if_ra_prop_get_value_all(const octave_value_list &args);
octave_value_list if_ra_prop_get_events(const octave_value_list &args);
octave_value_list if_ra_sum_add(const octave_value_list &args);
octave_value_list if_ra_sum_add_ch(const octave_value_list &args);
octave_value_list if_ra_sum_delete(const octave_value_list &args);
octave_value_list if_ra_sum_get(const octave_value_list &args);
octave_value_list if_ra_sum_add_part(const octave_value_list &args);
octave_value_list if_ra_sum_del_part(const octave_value_list &args);
octave_value_list if_ra_sum_get_part_all(const octave_value_list &args);
octave_value_list if_ra_sum_set_part_data(const octave_value_list &args);
octave_value_list if_ra_sum_get_part_data(const octave_value_list &args);
octave_value_list if_ra_sum_get_part_events(const octave_value_list &args);
octave_value_list if_ra_sum_set_part_events(const octave_value_list &args);

/******************** function infos ********************/
struct if_ra_func
{
	const char *func_name;
	int n_args_in;
	int n_args_out;
	octave_value_list (*func)(const octave_value_list &args);
};  /* struct if_ra_func */


struct if_ra_func f[] = {
	{ "ra_lib_init", 0, 1, if_ra_lib_init },
	{ "ra_lib_close", 1, 0, if_ra_lib_close },
	{ "ra_lib_get_error", 1, 2, if_ra_lib_get_error },
	{ "ra_lib_handle_from_any_handle", 1, 1, if_ra_lib_handle_from_any_handle },
	{ "ra_lib_set_option", 4, 1, if_ra_lib_set_option },
	{ "ra_lib_get_option", 2, 1, if_ra_lib_get_option },
	{ "ra_lib_info_get", 2, 3, if_ra_lib_info_get },
	{ "ra_plugin_info_get", 3, 3, if_ra_plugin_info_get },
	{ "ra_result_info_get", 3, 3, if_ra_result_info_get },
	{ "ra_meas_info_get", 2, 3, if_ra_meas_info_get },
	{ "ra_obj_info_get", 2, 3, if_ra_obj_info_get },
	{ "ra_rec_info_get", 2, 3, if_ra_rec_info_get },
	{ "ra_dev_info_get", 3, 3, if_ra_dev_info_get },
	{ "ra_ch_info_get", 3, 3, if_ra_ch_info_get },
	{ "ra_eval_info_get", 2, 3, if_ra_eval_info_get },
	{ "ra_class_info_get", 2, 3, if_ra_class_info_get },
	{ "ra_prop_info_get", 2, 3, if_ra_prop_info_get },
	{ "ra_summary_info_get", 2, 3, if_ra_summary_info_get },
	{ "ra_sum_dim_info_get", 3, 3, if_ra_sum_dim_info_get },
	{ "ra_sum_ch_info_get", 3, 3, if_ra_sum_ch_info_get },
	{ "ra_info_get_by_idx", 3, 3, if_ra_info_get_by_idx },
	{ "ra_info_set", 4, 1, if_ra_info_set },
	{ "ra_meas_handle_from_any_handle", 1, 1, if_ra_meas_handle_from_any_handle },
	{ "ra_meas_find", 2, 2, if_ra_meas_find },
	{ "ra_meas_open", 4, 1, if_ra_meas_open },
	{ "ra_meas_new", 3, 1, if_ra_meas_new },
	{ "ra_meas_save", 1, 0, if_ra_meas_save },
	{ "ra_meas_close", 1, 0, if_ra_meas_close },
	{ "ra_rec_get_first", 2, 1, if_ra_rec_get_first },
	{ "ra_rec_get_next", 1, 1, if_ra_rec_get_next },
	{ "ra_rec_get_first_child", 1, 1, if_ra_rec_get_first_child },
	{ "ra_raw_get", 4, 1, if_ra_raw_get },
	{ "ra_raw_get_unit", 4, 1, if_ra_raw_get_unit },
	{ "ra_meas_copy", 2, 1, if_ra_meas_copy },
	{ "ra_meas_move", 2, 1, if_ra_meas_move },
	{ "ra_meas_delete", 1, 1, if_ra_meas_delete },
	{ "ra_lib_use_plugin", 3, 0, if_ra_lib_use_plugin },
	{ "ra_plugin_get_by_num", 3, 1, if_ra_plugin_get_by_num },
	{ "ra_plugin_get_by_name", 3, 1, if_ra_plugin_get_by_name },
	{ "ra_proc_get", 2, 1, if_ra_proc_get },
	{ "ra_proc_do", 1, 1, if_ra_proc_do },
	{ "ra_proc_get_result_idx", 2, 1, if_ra_proc_get_result_idx },
	{ "ra_proc_get_result", 2, 3, if_ra_proc_get_result },
	{ "ra_proc_free", 1, 0, if_ra_proc_free },
	{ "ra_eval_save", 3, 1, if_ra_eval_save },
	{ "ra_eval_attribute_list", 1, 1, if_ra_eval_attribute_list },
	{ "ra_eval_attribute_get", 2, 1, if_ra_eval_attribute_get },
	{ "ra_eval_attribute_set", 4, 0, if_ra_eval_attribute_set },
	{ "ra_eval_attribute_unset", 2, 1, if_ra_eval_attribute_unset },
	{ "ra_eval_add", 4, 1, if_ra_eval_add },
	{ "ra_eval_delete", 1, 1, if_ra_eval_delete },
	{ "ra_eval_get_all", 1, 1, if_ra_eval_get_all },
	{ "ra_eval_get_original", 1, 1, if_ra_eval_get_original },
	{ "ra_eval_get_default", 1, 1, if_ra_eval_get_default },
	{ "ra_eval_set_default", 1, 1, if_ra_eval_set_default },
	{ "ra_eval_get_handle", 1, 1, if_ra_eval_get_handle },
	{ "ra_class_add", 4, 1, if_ra_class_add },
	{ "ra_class_add_predef", 2, 1, if_ra_class_add_predef },
	{ "ra_class_delete", 1, 1, if_ra_class_delete },
	{ "ra_class_get", 2, 1, if_ra_class_get },
	{ "ra_class_add_event", 3, 1, if_ra_class_add_event },
	{ "ra_class_del_event", 2, 1, if_ra_class_del_event },
	{ "ra_class_get_event_pos", 2, 1, if_ra_class_get_event_pos },
	{ "ra_class_set_event_pos", 4, 1, if_ra_class_set_event_pos },
	{ "ra_class_get_events", 5, 1, if_ra_class_get_events },
	{ "ra_class_get_prev_event", 2, 1, if_ra_class_get_prev_event },
	{ "ra_class_get_next_event", 2, 1, if_ra_class_get_next_event },
	{ "ra_class_get_handle", 1, 1, if_ra_class_get_handle },
	{ "ra_prop_add", 11, 1, if_ra_prop_add },
	{ "ra_prop_add_predef", 2, 1, if_ra_prop_add_predef },
	{ "ra_prop_delete", 1, 1, if_ra_prop_delete },
	{ "ra_prop_get", 2, 1, if_ra_prop_get },
	{ "ra_prop_get_all", 1, 1, if_ra_prop_get_all },
	{ "ra_prop_set_value", 5, 0, if_ra_prop_set_value },
	{ "ra_prop_set_value_mass", 5, 1, if_ra_prop_set_value_mass },
	{ "ra_prop_get_ch", 2, 1, if_ra_prop_get_ch },
	{ "ra_prop_get_value", 3, 1, if_ra_prop_get_value },
	{ "ra_prop_get_value_all", 1, 3, if_ra_prop_get_value_all },
	{ "ra_prop_get_events", 5, 1, if_ra_prop_get_events },
	{ "ra_sum_add", 7, 1, if_ra_sum_add },
	{ "ra_sum_add_ch", 3, 1, if_ra_sum_add_ch },
	{ "ra_sum_delete", 1, 1, if_ra_sum_delete },
	{ "ra_sum_get", 2, 1, if_ra_sum_get },
	{ "ra_sum_add_part", 2, 1, if_ra_sum_add_part },
	{ "ra_sum_del_part", 2, 1, if_ra_sum_del_part },
	{ "ra_sum_get_part_all", 1, 1, if_ra_sum_get_part_all },
	{ "ra_sum_set_part_data", 6, 1, if_ra_sum_set_part_data },
	{ "ra_sum_get_part_data", 4, 0, if_ra_sum_get_part_data },
	{ "ra_sum_get_part_events", 2, 0, if_ra_sum_get_part_events },
	{ "ra_sum_set_part_events", 3, 1, if_ra_sum_set_part_events },
};  /* struct if_ra_func f */

#endif  /* RA_OCTAVE_H */
