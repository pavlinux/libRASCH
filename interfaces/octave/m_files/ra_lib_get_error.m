function [error_number, error_text] = ra_lib_get_error(ra_handle)
% Returns the last error occured inside of libRASCH.
%   Input parameters:
%     ra_handle
% 
%   Output parameters:
%     error_number
%     error_text

	[error_number, error_text] = ra_octave('ra_lib_get_error', ra_handle);
