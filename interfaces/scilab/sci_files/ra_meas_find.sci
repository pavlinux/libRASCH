function [signals, plugins] = ra_meas_find(ra_handle, directory)
// Returns all signales handled by libRASCH in the directory 'directory'.
//   Input parameters:
//     ra_handle
//     directory
// 
//   Output parameters:
//     signals
//     plugins

	[signals, plugins] = ra_scilab('ra_meas_find', ra_handle, directory);
