function [eval_handle] = ra_eval_get_original(meas_handle)
// Returns the evaluation-handle of the original evaluation.
//     Input parameters:
//     meas_handle
// 
//   Output parameters:
//     eval_handle

	[eval_handle] = ra_scilab('ra_eval_get_original', meas_handle);
