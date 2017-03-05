function [event_ids] = ra_prop_get_events(prop_handle, val_type, min, max, ch)
// Return the event-ids which values are between 'min' and 'max.
//   Input parameters:
//     prop_handle
//     val_type
//     min
//     max
//     ch
// 
//   Output parameters:
//     event_ids

	[event_ids] = ra_scilab('ra_prop_get_events', prop_handle, val_type, min, max, ch);
