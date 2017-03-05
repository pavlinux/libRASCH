function [value, name, desc] = ra_plugin_get_info(plugin_handle, info_name)
// Returns the information 'info_name' about the plugin
// 'plugin_handle'. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     plugin_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_plugin_get_info', plugin_handle, info_name);
