function [value, name, desc] = ra_ch_info_get(rec_handle, channel, info_name)
% Returns the information 'info_name' about a
% recorded channel. The name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     rec_handle
%     channel
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_octave('ra_ch_info_get', rec_handle, channel, info_name);
