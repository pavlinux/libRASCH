function [prop_handle] = ra_prop_add_predef(class_handle, id)
% Returns the handle of the added event-property.
%     Input parameters:
%     class_handle
%     id
% 
%   Output parameters:
%     prop_handle

	[prop_handle] = ra_matlab('ra_prop_add_predef', class_handle, id);
