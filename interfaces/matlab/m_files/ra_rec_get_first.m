function [rec_handle] = ra_rec_get_first(meas_handle, session_number)
% Returns the first recording-handle for session number
% 'session_number'.
%   Input parameters:
%     meas_handle
%     session_number
% 
%   Output parameters:
%     rec_handle

	[rec_handle] = ra_matlab('ra_rec_get_first', meas_handle, session_number);
