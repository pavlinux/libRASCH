function [plugin_handle] = ra_plugin_get_by_name(ra_handle, name, search_all)
// Returns the plugin-handle for plugin 'name'.
//   Input parameters:
//     ra_handle
//     name
//     search_all
// 
//   Output parameters:
//     plugin_handle

	[plugin_handle] = ra_scilab('ra_plugin_get_by_name', ra_handle, name, search_all);
