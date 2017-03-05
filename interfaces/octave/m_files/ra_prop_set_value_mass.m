function [ret] = ra_prop_set_value_mass(prop_handle, event_id, ch, value_type, value)
% 
% Set all values of an event-property.
%   Input parameters:
%     prop_handle
%     event_id
%     ch
%     value_type
%     value
% 
%   Output parameters:
%     ret

	[ret] = ra_octave('ra_prop_set_value_mass', prop_handle, event_id, ch, value_type, value);
