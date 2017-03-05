function [eval_handle] = ra_eval_get_orig(meas_handle)
// Returns the evaluation-handle from the evaluation
// performed from the "orginial" recording system. Returns '0' if there
// is not an original evaluation.
//   Input parameters:
//     meas_handle
// 
//   Output parameters:
//     eval_handle

	[eval_handle] = ra_scilab('ra_eval_get_orig', meas_handle);
