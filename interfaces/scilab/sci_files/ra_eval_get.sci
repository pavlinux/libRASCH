function [eval_handle] = ra_eval_get(meas_handle, eval_number)
// Returns the evaluation-handle from evaluation number
// 'eval_number'.
//   Input parameters:
//     meas_handle
//     eval_number
// 
//   Output parameters:
//     eval_handle

	[eval_handle] = ra_scilab('ra_eval_get', meas_handle, eval_number);
