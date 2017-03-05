function [eval_handle] = ra_eval_add(meas_handle, name, desc, orig)
% Returns the evaluation-handle of the newly added evaluation.
%     Input parameters:
%     meas_handle
%     name
%     desc
%     orig
% 
%   Output parameters:
%     eval_handle

	[eval_handle] = ra_octave('ra_eval_add', meas_handle, name, desc, orig);
