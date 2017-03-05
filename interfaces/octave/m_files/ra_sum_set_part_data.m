function [ok] = ra_sum_set_part_data(sum_handle, part_id, ch_idx, dim, data_type, data)
% 
% Set the data for an event-summary.
%   Input parameters:
%     sum_handle
%     part_id
%     ch_idx
%     dim
%     data_type
%     data
% 
%   Output parameters:
%     ok

	[ok] = ra_octave('ra_sum_set_part_data', sum_handle, part_id, ch_idx, dim, data_type, data);
