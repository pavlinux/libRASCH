#!/bin/sh

#
# Apply the API function name changes done for libRASCH 0.6.0
#
# Usage: sh do_api_changes_for_0.6.0 file [file ... ]
#
# ATTENTION:
# Make a backup of your files before running this script on your
# source. The changes are done _in-place_ in all the files given
# as arguments to the script. 
#

perl -pi -e "s/ra_init\b/ra_lib_init/g;" $*
perl -pi -e "s/ra_close\b/ra_lib_close/g;" $*
perl -pi -e "s/ra_get_error\b/ra_lib_get_error/g;" $*
perl -pi -e "s/ra_handle_from_any_handle\b/ra_lib_handle_from_any_handle/g;" $*
perl -pi -e "s/ra_use_plugin\b/ra_lib_use_plugin/g;" $*
perl -pi -e "s/ra_plugin_by_num\b/ra_plugin_get_by_num/g;" $*
perl -pi -e "s/ra_plugin_by_name\b/ra_plugin_get_by_name/g;" $*
perl -pi -e "s/ra_find_first_meas\b/ra_meas_find_first/g;" $*
perl -pi -e "s/ra_find_next_meas\b/ra_meas_find_next/g;" $*
perl -pi -e "s/ra_close_find\b/ra_meas_close_find/g;" $*
perl -pi -e "s/ra_open_meas\b/ra_meas_open/g;" $*
perl -pi -e "s/ra_new_meas\b/ra_meas_new/g;" $*
perl -pi -e "s/ra_save_meas\b/ra_meas_save/g;" $*
perl -pi -e "s/ra_close_meas\b/ra_meas_close/g;" $*
perl -pi -e "s/ra_get_info_id\b/ra_info_get/g;" $*
perl -pi -e "s/ra_get_info_idx\b/ra_info_get_by_idx/g;" $*
perl -pi -e "s/ra_set_info\b/ra_info_set/g;" $*
perl -pi -e "s/ra_get_first_session_rec\b/ra_rec_get_first/g;" $*
perl -pi -e "s/ra_get_next_rec\b/ra_rec_get_next/g;" $*
perl -pi -e "s/ra_get_first_child_rec\b/ra_rec_get_first_child/g;" $*
perl -pi -e "s/ra_get_raw_b\b/ra_raw_get_b/g;" $*
perl -pi -e "s/ra_get_raw_w\b/ra_raw_get_w/g;" $*
perl -pi -e "s/ra_get_raw_dw\b/ra_raw_get_dw/g;" $*
perl -pi -e "s/ra_get_raw_d\b/ra_raw_get_d/g;" $*
perl -pi -e "s/ra_call_gui\b/ra_gui_call/g;" $*
perl -pi -e "s/ra_get_proc_handle\b/ra_proc_get/g;" $*
perl -pi -e "s/ra_free_proc_handle\b/ra_proc_free/g;" $*
perl -pi -e "s/ra_do_processing\b/ra_proc_do/g;" $*
perl -pi -e "s/ra_get_result\b/ra_proc_get_result/g;" $*
perl -pi -e "s/ra_get_view\b/ra_view_get/g;" $*
perl -pi -e "s/ra_value_type\b/ra_value_get_type/g;" $*
perl -pi -e "s/ra_value_n_elem\b/ra_value_get_num_elem/g;" $*
perl -pi -e "s/ra_value_info\b/ra_value_get_info/g;" $*
perl -pi -e "s/ra_value_name\b/ra_value_get_name/g;" $*
perl -pi -e "s/ra_value_desc\b/ra_value_get_desc/g;" $*
perl -pi -e "s/ra_value_long\b/ra_value_set_long/g;" $*
perl -pi -e "s/ra_value_double\b/ra_value_set_double/g;" $*
perl -pi -e "s/ra_value_string\b/ra_value_set_string/g;" $*
perl -pi -e "s/ra_value_long_array\b/ra_value_set_long_array/g;" $*
perl -pi -e "s/ra_value_double_array\b/ra_value_set_double_array/g;" $*
perl -pi -e "s/ra_value_string_array\b/ra_value_set_string_array/g;" $*
perl -pi -e "s/ra_get_option\b/ra_lib_get_option/g;" $*
perl -pi -e "s/ra_set_option\b/ra_lib_set_option/g;" $*
perl -pi -e "s/ra_copy\b/ra_meas_copy/g;" $*
perl -pi -e "s/ra_move\b/ra_meas_move/g;" $*
perl -pi -e "s/ra_delete\b/ra_meas_delete/g;" $*
perl -pi -e "s/ra_eval_del\b/ra_eval_delete/g;" $*
perl -pi -e "s/ra_evset_by_num\b/ra_evset_get_by_num/g;" $*
perl -pi -e "s/ra_evset_by_name\b/ra_evset_get_by_name/g;" $*
perl -pi -e "s/ra_evset_by_evprop\b/ra_evset_get_by_prop/g;" $*
perl -pi -e "s/ra_ev_get_num\b/ra_evset_get_num_events/g;" $*
perl -pi -e "s/ra_evprop_by_num\b/ra_prop_get_by_num/g;" $*
perl -pi -e "s/ra_evprop_by_name\b/ra_prop_get_by_name/g;" $*

perl -pi -e "s/ra_evprop_add\b/ra_prop_add/g;" $*
perl -pi -e "s/ra_evprop_add_name\b/ra_prop_add_name/g;" $*
perl -pi -e "s/ra_evprop_add_custom\b/ra_prop_add_custom/g;" $*
perl -pi -e "s/ra_evprop_del\b/ra_prop_del/g;" $*

perl -pi -e "s/ra_evset_get_flag_evprop\b/ra_evset_get_flag_prop/g;" $*

perl -pi -e "s/evprop_handle/prop_handle/g;" $*

perl -pi -e "s/if_ra_get_lib_info\b/if_ra_lib_get_info/g;" $*
perl -pi -e "s/if_ra_get_plugin_info\b/if_ra_plugin_get_info/g;" $*
perl -pi -e "s/if_ra_get_result_info\b/if_ra_result_get_info/g;" $*
perl -pi -e "s/if_ra_get_meas_info\b/if_ra_meas_get_info/g;" $*
perl -pi -e "s/if_ra_get_obj_info\b/if_ra_obj_get_info/g;" $*
perl -pi -e "s/if_ra_get_rec_info\b/if_ra_rec_get_info/g;" $*
perl -pi -e "s/if_ra_get_dev_info\b/if_ra_dev_get_info/g;" $*
perl -pi -e "s/if_ra_get_ch_info\b/if_ra_ch_get_info/g;" $*
perl -pi -e "s/if_ra_get_eval_info\b/if_ra_eval_get_info/g;" $*
perl -pi -e "s/if_ra_get_evset_info\b/if_ra_evset_get_info/g;" $*
perl -pi -e "s/if_ra_get_evprop_info\b/if_ra_prop_get_info/g;" $*
perl -pi -e "s/if_ra_init_processing\b/if_ra_proc_init/g;" $*
perl -pi -e "s/if_ra_end_processing\b/if_ra_proc_end/g;" $*
perl -pi -e "s/if_ra_get_raw\b/if_ra_raw_get/g;" $*
perl -pi -e "s/if_ra_get_results\b/if_ra_result_get/g;" $*
perl -pi -e "s/if_ra_get_all_evprops\b/if_ra_prop_get_all/g;" $*
perl -pi -e "s/if_ra_find_meas\b/if_ra_meas_find/g;" $*

perl -pi -e "s/ra_get_lib_info\b/ra_lib_get_info/g;" $*
perl -pi -e "s/ra_get_plugin_info\b/ra_plugin_get_info/g;" $*
perl -pi -e "s/ra_get_result_info\b/ra_result_get_info/g;" $*
perl -pi -e "s/ra_get_meas_info\b/ra_meas_get_info/g;" $*
perl -pi -e "s/ra_get_obj_info\b/ra_obj_get_info/g;" $*
perl -pi -e "s/ra_get_rec_info\b/ra_rec_get_info/g;" $*
perl -pi -e "s/ra_get_dev_info\b/ra_dev_get_info/g;" $*
perl -pi -e "s/ra_get_ch_info\b/ra_ch_get_info/g;" $*
perl -pi -e "s/ra_get_eval_info\b/ra_eval_get_info/g;" $*
perl -pi -e "s/ra_get_evset_info\b/ra_evset_get_info/g;" $*
perl -pi -e "s/ra_get_evprop_info\b/ra_prop_get_info/g;" $*
perl -pi -e "s/ra_init_processing\b/ra_proc_init/g;" $*
perl -pi -e "s/ra_end_processing\b/ra_proc_end/g;" $*
perl -pi -e "s/ra_get_raw\b/ra_raw_get/g;" $*
perl -pi -e "s/ra_get_results\b/ra_result_get/g;" $*
perl -pi -e "s/ra_get_all_evprops\b/ra_prop_get_all/g;" $*
perl -pi -e "s/ra_find_meas\b/ra_meas_find/g;" $*
