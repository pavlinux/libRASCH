function [rec_handle] = ra_rec_get_next(rec_handle)
% Returns the recording-handle after 'rec_handle'.
%   Input parameters:
%     rec_handle
% 
%   Output parameters:
%     rec_handle

	[rec_handle] = ra_octave('ra_rec_get_next', rec_handle);
