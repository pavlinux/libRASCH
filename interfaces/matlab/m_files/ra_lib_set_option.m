function [ret] = ra_lib_set_option(handle, option_name, option_type, option_value)
% 
% Set an option.
%   Input parameters:
%     handle
%     option_name
%     option_type
%     option_value
% 
%   Output parameters:
%     ret

	[ret] = ra_matlab('ra_lib_set_option', handle, option_name, option_type, option_value);
