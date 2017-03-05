function [value, name, desc] = ra_sum_ch_info_get(sum_handle, channel, info_name)
% Returns the information 'info_name' about the channel
% 'channel' of the event-summary. The name and a short description is
% also returned. See XXX for available informations.
%   Input parameters:
%     sum_handle
%     channel
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_matlab('ra_sum_ch_info_get', sum_handle, channel, info_name);
