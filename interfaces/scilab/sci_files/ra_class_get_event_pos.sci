function [pos] = ra_class_get_event_pos(class_handle, event_id)
// Returns the start- and end-position of the event 'event_id'.
//     Input parameters:
//     class_handle
//     event_id
// 
//   Output parameters:
//     pos

	[pos] = ra_scilab('ra_class_get_event_pos', class_handle, event_id);
