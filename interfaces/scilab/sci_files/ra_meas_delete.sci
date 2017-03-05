function [ok] = ra_meas_delete(meas_handle)
// 
// The function deletes the files of the measurement. The measurement
// will be closed after the deletion. Therefore meas_handle is no longer a valid
// measurement-handle.
// 
// IMPORTANT!!!
// Function _not_ completely tested. Use at your own risk.
//   Input parameters:
//     meas_handle
// 
//   Output parameters:
//     ok

	[ok] = ra_scilab('ra_meas_delete', meas_handle);
