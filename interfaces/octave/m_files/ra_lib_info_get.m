function [value, name, desc] = ra_lib_info_get(ra_handle, info_name)
% Returns the information 'info_name' about libRASCH. The
% name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     ra_handle
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_octave('ra_lib_info_get', ra_handle, info_name);
