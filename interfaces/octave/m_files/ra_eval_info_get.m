function [value, name, desc] = ra_eval_info_get(eval_handle, info_name)
% Returns the information 'info_name' about an evaluation.
% The name and a short description is also returned. See
% XXX for available informations.
%   Input parameters:
%     eval_handle
%     info_name
% 
%   Output parameters:
%     value
%     name
%     desc

	[value, name, desc] = ra_octave('ra_eval_info_get', eval_handle, info_name);
