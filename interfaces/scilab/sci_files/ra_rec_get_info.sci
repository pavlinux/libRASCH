function [value, name, desc] = ra_rec_get_info(rec_handle, info_name)
// Returns the information 'info_name' about a
// recording. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     rec_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_rec_get_info', rec_handle, info_name);
