function [eval_handle] = ra_eval_get_default(meas_handle)
// Returns the evaluation-handle of the default evaluation.
//     Input parameters:
//     meas_handle
// 
//   Output parameters:
//     eval_handle

	[eval_handle] = ra_scilab('ra_eval_get_default', meas_handle);
