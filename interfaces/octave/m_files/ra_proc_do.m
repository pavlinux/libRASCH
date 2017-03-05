function [error] = ra_proc_do(process_handle)
% Perform some calculation using the processing-plugin
% 'plugin_handle. The function returns a process-handle, which will be
% used in the function 'ra_result_get' to get the results of the
% calculation.
%   Input parameters:
%     process_handle
% 
%   Output parameters:
%     error

	[error] = ra_octave('ra_proc_do', process_handle);
