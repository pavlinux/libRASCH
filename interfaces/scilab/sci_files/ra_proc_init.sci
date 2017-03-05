function [process_handle] = ra_proc_init(ra_handle, meas_handle, rec_handle, eval_handle, plugin_handle, save_in_eval)
// 
//   Input parameters:
//     ra_handle
//     meas_handle
//     rec_handle
//     eval_handle
//     plugin_handle
//     save_in_eval
// 
//   Output parameters:
//     process_handle

	[process_handle] = ra_scilab('ra_proc_init', ra_handle, meas_handle, rec_handle, eval_handle, plugin_handle, save_in_eval);
