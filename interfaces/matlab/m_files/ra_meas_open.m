function [meas_handle] = ra_meas_open(ra_handle, signal_name, eval_file, open_fast)
% Opens the measurement 'signal_name'. If you are
% interested only in information about the type of the measurement or
% about informations of the measurement-object than the flag 'open_fast'
% can be set to '1' and the opening is faster (the reading of the
% evaluations are skipped.) A specific evaluation file can be given
% in 'eval_file'. If this is set to NULL or the empty string, libRASCH
% will look for the default evaluation file associated with the
% measurement.
%   Input parameters:
%     ra_handle
%     signal_name
%     eval_file
%     open_fast
% 
%   Output parameters:
%     meas_handle

	[meas_handle] = ra_matlab('ra_meas_open', ra_handle, signal_name, eval_file, open_fast);
