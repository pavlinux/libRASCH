function [data] = ra_raw_get(rec_handle, channel, start_sample, num_samples)
% Returns 'num_samples' raw-data starting with sample
% 'start_sample' from recording 'rec_handle' and channel 'ch'.
%   Input parameters:
%     rec_handle
%     channel
%     start_sample
%     num_samples
% 
%   Output parameters:
%     data

	[data] = ra_octave('ra_raw_get', rec_handle, channel, start_sample, num_samples);
