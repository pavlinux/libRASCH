function [eval_handle] = ra_eval_get_handle(class_handle)
% Returns the evaluation-handle the event-class 'class_handle' belongs to.
%     Input parameters:
%     class_handle
% 
%   Output parameters:
%     eval_handle

	[eval_handle] = ra_matlab('ra_eval_get_handle', class_handle);
