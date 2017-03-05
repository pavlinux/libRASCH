function [meas_handle] = ra_meas_new(ra_handle, dir_name, signal_name)
// Creates a new measurement with the name @name in the
// directory @dir. The function returns the handle to the
// measurement. The measurement will be stored using the libRASCH
// file-format.
//   Input parameters:
//     ra_handle
//     dir_name
//     signal_name
// 
//   Output parameters:
//     meas_handle

	[meas_handle] = ra_scilab('ra_meas_new', ra_handle, dir_name, signal_name);
