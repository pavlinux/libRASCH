function [meas_handle] = ra_meas_handle_from_any_handle(any_handle)
% Returns a measurement-handle from any handle.
%   Input parameters:
%     any_handle
% 
%   Output parameters:
%     meas_handle

	[meas_handle] = ra_octave('ra_meas_handle_from_any_handle', any_handle);
