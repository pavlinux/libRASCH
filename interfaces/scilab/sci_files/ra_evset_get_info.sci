function [value, name, desc] = ra_evset_get_info(evset_handle, info_name)
// Returns the information 'info_name' about an
// evaluation-set. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     evset_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_evset_get_info', evset_handle, info_name);
