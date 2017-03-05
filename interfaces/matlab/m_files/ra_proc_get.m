function [process_handle] = ra_proc_get(meas_handle, plugin_handle)
% 
%   Input parameters:
%     meas_handle
%     plugin_handle
% 
%   Output parameters:
%     process_handle

	[process_handle] = ra_matlab('ra_proc_get', meas_handle, plugin_handle);
