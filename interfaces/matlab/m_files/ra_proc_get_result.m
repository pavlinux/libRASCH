function [values, name, desc] = ra_proc_get_result(process_handle, plugin_handle)
% Returns the results of the calculation done calling
% 'ra_proc_do'. It also returns the name's and short descriptions
% of the values. The returning variables are vectors containing all
% results.
%   Input parameters:
%     process_handle
%     plugin_handle
% 
%   Output parameters:
%     values
%     name
%     desc

	[values, name, desc] = ra_matlab('ra_proc_get_result', process_handle, plugin_handle);
