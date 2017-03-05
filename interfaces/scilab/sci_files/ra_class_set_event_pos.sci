function [OK] = ra_class_set_event_pos(class_handle, event_id, start_pos, end_pos)
// Set the start- and end-position of the event 'event_id'.
//     Input parameters:
//     class_handle
//     event_id
//     start_pos
//     end_pos
// 
//   Output parameters:
//     OK

	[OK] = ra_scilab('ra_class_set_event_pos', class_handle, event_id, start_pos, end_pos);
