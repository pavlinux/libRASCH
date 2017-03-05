function [event_id] = ra_class_get_prev_event(class_handle, event_id)
// Returns the event-id of the previous event.
//     Input parameters:
//     class_handle
//     event_id
// 
//   Output parameters:
//     event_id

	[event_id] = ra_scilab('ra_class_get_prev_event', class_handle, event_id);
