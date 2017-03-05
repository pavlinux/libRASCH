function [signals, plugins] = ra_meas_find(ra_handle, directory)
% Returns all signales handled by libRASCH in the directory 'directory'.
%   Input parameters:
%     ra_handle
%     directory
% 
%   Output parameters:
%     signals
%     plugins


    [fn, pl] = ra_octave('ra_meas_find', ra_handle, directory);
    [m,n] = size(fn);
    signals = cell(m,1);
    for i=1:m
        signals{i} = fn(i,:);
    endfor
    plugins = cell(m,1);
    for i=1:m
        plugins{i} = pl(i,:);
    endfor

