function [event_ids, ch, values] = ra_prop_get_value_all(prop_handle)
// Returns all event-property values and the corresponding event-ids channels.
//   Input parameters:
//     prop_handle
// 
//   Output parameters:
//     event_ids
//     ch
//     values

	[event_ids, ch, values] = ra_scilab('ra_prop_get_value_all', prop_handle);
