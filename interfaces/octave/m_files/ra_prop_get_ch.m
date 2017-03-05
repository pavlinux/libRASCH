function [ch] = ra_prop_get_ch(prop_handle, id)
% Returns for event 'event-id' a list of channels for which
% values are available.
%   Input parameters:
%     prop_handle
%     id
% 
%   Output parameters:
%     ch

	[ch] = ra_octave('ra_prop_get_ch', prop_handle, id);
