function [ok] = ra_sum_delete(sum_handle)
% Delete the event-summary given by 'sum_handle'.
%     Input parameters:
%     sum_handle
% 
%   Output parameters:
%     ok

	[ok] = ra_matlab('ra_sum_delete', sum_handle);
