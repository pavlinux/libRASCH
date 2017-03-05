function [part_id] = ra_sum_add_part(sum_handle, events)
% Add a summary part; the part-id is returned.  
%     Input parameters:
%     sum_handle
%     events
% 
%   Output parameters:
%     part_id

	[part_id] = ra_octave('ra_sum_add_part', sum_handle, events);
