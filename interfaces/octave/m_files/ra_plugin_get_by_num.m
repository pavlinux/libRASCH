function [plugin_handle] = ra_plugin_get_by_num(ra_handle, number, search_all)
% Returns the plugin-handle for plugin number 'number'.
%   Input parameters:
%     ra_handle
%     number
%     search_all
% 
%   Output parameters:
%     plugin_handle

	[plugin_handle] = ra_octave('ra_plugin_get_by_num', ra_handle, number, search_all);
