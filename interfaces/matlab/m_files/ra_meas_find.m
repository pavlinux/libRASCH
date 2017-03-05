function [signals, plugins] = ra_meas_find(ra_handle, directory)
% Returns all signales handled by libRASCH in the directory 'directory'.
%   Input parameters:
%     ra_handle
%     directory
% 
%   Output parameters:
%     signals
%     plugins


    [fn, pl] = ra_matlab('ra_meas_find', ra_handle, directory);
    signals = cellstr(fn);
    plugins = cellstr(pl);

