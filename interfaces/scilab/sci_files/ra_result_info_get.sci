function [value, name, desc] = ra_result_info_get(plugin_handle, result_num, info_name)
// Returns the information 'info_name' about the result
// number 'result_num' from plugin
// 'plugin_handle'. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     plugin_handle
//     result_num
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_result_info_get', plugin_handle, result_num, info_name);
