function [class_handle] = ra_class_add(eval_handle, id, name, desc)
% Returns the handle of the newly added event-class.
%     Input parameters:
%     eval_handle
%     id
%     name
%     desc
% 
%   Output parameters:
%     class_handle

	[class_handle] = ra_matlab('ra_class_add', eval_handle, id, name, desc);
