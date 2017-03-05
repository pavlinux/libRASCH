function [option_value] = ra_lib_get_option(handle, option_name)
% 
% Get an option.
%   Input parameters:
%     handle
%     option_name
% 
%   Output parameters:
%     option_value

	[option_value] = ra_octave('ra_lib_get_option', handle, option_name);
