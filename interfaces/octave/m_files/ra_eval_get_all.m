function [value] = ra_eval_get_all(meas_handle)
% 
%     Returns all evaluations belonging to 'meas_handle'.
%     Input parameters:
%     meas_handle
% 
%   Output parameters:
%     value

	[value] = ra_octave('ra_eval_get_all', meas_handle);
