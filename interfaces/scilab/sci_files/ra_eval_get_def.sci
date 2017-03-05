function [eval_handle] = ra_eval_get_def(meas_handle)
// Returns the default evaluation-handle.
//   Input parameters:
//     meas_handle
// 
//   Output parameters:
//     eval_handle

	[eval_handle] = ra_scilab('ra_eval_get_def', meas_handle);
