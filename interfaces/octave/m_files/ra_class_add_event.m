function [event_id] = ra_class_add_event(class_handle, start_pos, end_pos)
% Returns the id of the added event.
%     Input parameters:
%     class_handle
%     start_pos
%     end_pos
% 
%   Output parameters:
%     event_id

	[event_id] = ra_octave('ra_class_add_event', class_handle, start_pos, end_pos);
