function [ok] = ra_eval_save(meas_handle, file, use_ascii)
// Save evaluation infos of a measurement.
//     Input parameters:
//     meas_handle
//     file
//     use_ascii
// 
//   Output parameters:
//     ok

	[ok] = ra_scilab('ra_eval_save', meas_handle, file, use_ascii);
