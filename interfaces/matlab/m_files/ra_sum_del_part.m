function [ok] = ra_sum_del_part(sum_handle, part_id)
% Delete a summary part.
%     Input parameters:
%     sum_handle
%     part_id
% 
%   Output parameters:
%     ok

	[ok] = ra_matlab('ra_sum_del_part', sum_handle, part_id);
