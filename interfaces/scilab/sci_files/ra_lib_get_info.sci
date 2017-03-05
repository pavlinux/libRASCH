function [value, name, desc] = ra_lib_get_info(ra_handle, info_name)
// Returns the information 'info_name' about libRASCH. The
// name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     ra_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_lib_get_info', ra_handle, info_name);
