function [ok] = ra_eval_set_default(eval_handle)
% Set the evaluation 'eval_handle' as the default one.
%     Input parameters:
%     eval_handle
% 
%   Output parameters:
%     ok

	[ok] = ra_octave('ra_eval_set_default', eval_handle);
