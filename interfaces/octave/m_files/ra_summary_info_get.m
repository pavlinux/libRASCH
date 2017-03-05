function [value, name, desc] = ra_summary_info_get(sum_handle, info_name)
% Returns the information 'info_name' about an
% event-summary. The name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     sum_handle
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_octave('ra_summary_info_get', sum_handle, info_name);
