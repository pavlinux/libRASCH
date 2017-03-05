function [value, name, desc] = ra_info_get_by_idx(any_handle, info_type, index)
% Returns the information number 'index' from the info-type
% 'info_type'. The name and a short description is also returned. See
% XXX for available informations and for valid 'info_type'.
%   Input parameters:
%     any_handle
%     info_type
%     index
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_matlab('ra_info_get_by_idx', any_handle, info_type, index);
