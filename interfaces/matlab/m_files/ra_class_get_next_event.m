function [event_id] = ra_class_get_next_event(class_handle, event_id)
% Returns the event-id of the next event.
%     Input parameters:
%     class_handle
%     event_id
% 
%   Output parameters:
%     event_id

	[event_id] = ra_matlab('ra_class_get_next_event', class_handle, event_id);
