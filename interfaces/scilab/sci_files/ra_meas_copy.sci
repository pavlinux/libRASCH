function [ok] = ra_meas_copy(meas_handle, dir)
// 
// The function copy the files of measurement meas_handle to directory dir.
// 
// IMPORTANT!!!
// Function _not_ completely tested. Use at your own risk.
//   Input parameters:
//     meas_handle
//     dir
// 
//   Output parameters:
//     ok

	[ok] = ra_scilab('ra_meas_copy', meas_handle, dir);
