#ifndef RA_MATLAB_H
#define RA_MATLAB_H

/*
 * Don't change this file. It was created automatically and every change
 * you made will be lost after the next make.
 *
 * created using make_interfaces.pl Version 0.2.5  15.09.2008

 */


/******************** prototypes ********************/
void if_ra_lib_init(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_close(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_get_error(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_handle_from_any_handle(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_set_option(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_get_option(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_plugin_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_result_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_obj_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_rec_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_dev_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_ch_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_summary_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_dim_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_ch_info_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_info_get_by_idx(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_info_set(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_handle_from_any_handle(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_find(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_open(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_new(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_save(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_close(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_rec_get_first(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_rec_get_next(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_rec_get_first_child(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_raw_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_raw_get_unit(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_copy(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_move(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_meas_delete(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_lib_use_plugin(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_plugin_get_by_num(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_plugin_get_by_name(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_proc_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_proc_do(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_proc_get_result_idx(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_proc_get_result(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_proc_free(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_save(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_attribute_list(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_attribute_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_attribute_set(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_attribute_unset(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_add(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_delete(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_get_all(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_get_original(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_get_default(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_set_default(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_eval_get_handle(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_add(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_add_predef(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_delete(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_add_event(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_del_event(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get_event_pos(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_set_event_pos(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get_events(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get_prev_event(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get_next_event(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_class_get_handle(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_add(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_add_predef(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_delete(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get_all(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_set_value(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_set_value_mass(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get_ch(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get_value(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get_value_all(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_prop_get_events(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_add(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_add_ch(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_delete(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_get(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_add_part(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_del_part(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_get_part_all(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_set_part_data(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_get_part_data(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_get_part_events(mxArray *plhs[], const mxArray *prhs[]);
void if_ra_sum_set_part_events(mxArray *plhs[], const mxArray *prhs[]);

/******************** function infos ********************/
struct if_ra_func
{
	const char *func_name;
	int n_args_in;
	int n_args_out;
	void (*func)(mxArray *plhs[], const mxArray *prhs[]);
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

#endif  /* RA_MATLAB_H */
