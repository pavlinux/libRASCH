function [ok] = ra_info_set(handle, info_name, info_type, info_value)
% Set the information 'info_name' to 'info_value' for the object
% handled by 'handle'.
%   Input parameters:
%     handle
%     info_name
%     info_type
%     info_value
% 
%   Output parameters:
%     ok

	[ok] = ra_octave('ra_info_set', handle, info_name, info_type, info_value);
