function [value, name, desc] = ra_obj_info_get(meas_handle, info_name)
% Returns the information 'info_name' about a
% measurement-object. The name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     meas_handle
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_octave('ra_obj_info_get', meas_handle, info_name);
