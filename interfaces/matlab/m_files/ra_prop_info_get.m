function [value, name, desc] = ra_prop_info_get(prop_handle, info_name)
% Returns the information 'info_name' about an
% event-property. The name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     prop_handle
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_matlab('ra_prop_info_get', prop_handle, info_name);
