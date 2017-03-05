function [class_handle] = ra_class_add_predef(eval_handle, id)
% Returns the handle of the newly added event-class.
%     Input parameters:
%     eval_handle
%     id
% 
%   Output parameters:
%     class_handle

	[class_handle] = ra_matlab('ra_class_add_predef', eval_handle, id);
