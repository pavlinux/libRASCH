function [ra_handle] = ra_lib_init()
% Initialize libRASCH and read all plugins. Must be called
%     before any other function for libRASCH can be used.
%     Input parameters:
% 
%   Output parameters:
%     ra_handle

	[ra_handle] = ra_matlab('ra_lib_init');
