function [value, name, desc] = ra_prop_get_info(prop_handle, info_name)
// Returns the information 'info_name' about an
// event-property. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     prop_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_prop_get_info', prop_handle, info_name);
