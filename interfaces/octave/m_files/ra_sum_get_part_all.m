function [value] = ra_sum_get_part_all(sum_handle)
% 
%     Returns all parts in the event-summary 'sum_handle'.
%     Input parameters:
%     sum_handle
% 
%   Output parameters:
%     value

	[value] = ra_octave('ra_sum_get_part_all', sum_handle);
