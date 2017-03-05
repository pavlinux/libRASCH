function [ra_handle] = ra_lib_handle_from_any_handle(any_handle)
% Returns the ra-handle from any handle.
%   Input parameters:
%     any_handle
% 
%   Output parameters:
%     ra_handle

	[ra_handle] = ra_matlab('ra_lib_handle_from_any_handle', any_handle);
