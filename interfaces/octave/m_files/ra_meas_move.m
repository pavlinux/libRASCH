function [meas_handle_moved] = ra_meas_move(meas_handle, dir)
% 
% The function move the files of the measurement to directory dir. After the move
% the measurement will be reopend (to be able to handle the new positions of the files)
% and the functions returns the new measurement-handle.
% 
% IMPORTANT!!!
% Function _not_ completely tested. Use at your own risk.
%   Input parameters:
%     meas_handle
%     dir
% 
%   Output parameters:
%     meas_handle_moved

	[meas_handle_moved] = ra_octave('ra_meas_move', meas_handle, dir);
