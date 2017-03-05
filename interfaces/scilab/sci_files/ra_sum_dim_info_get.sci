function [value, name, desc] = ra_sum_dim_info_get(sum_handle, dim, info_name)
// Returns the information 'info_name' about the dimension
// 'dim' of the event-summary. The name and a short description is also
// returned. See XXX for available informations.
//   Input parameters:
//     sum_handle
//     dim
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_sum_dim_info_get', sum_handle, dim, info_name);
