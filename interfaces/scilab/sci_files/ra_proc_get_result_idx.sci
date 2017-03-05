function [index] = ra_proc_get_result_idx(process_handle, res_ascii_id)
// The function returns the index of the result
// 'res_ascii_id' in the results returned in re_proc_get_result().
//   Input parameters:
//     process_handle
//     res_ascii_id
// 
//   Output parameters:
//     index

	[index] = ra_scilab('ra_proc_get_result_idx', process_handle, res_ascii_id);
