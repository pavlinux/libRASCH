function [value] = ra_prop_get_value(prop_handle, id, ch)
% Returns the event-property value for event 'event-id' and channel 'channel'.
%   Input parameters:
%     prop_handle
%     id
%     ch
% 
%   Output parameters:
%     value

	[value] = ra_octave('ra_prop_get_value', prop_handle, id, ch);
