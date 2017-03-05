function [prop_handle] = ra_prop_get_by_name(eval_handle, name)
// Returns the event-property-handle with the name 'name'.
//   Input parameters:
//     eval_handle
//     name
// 
//   Output parameters:
//     prop_handle

	[prop_handle] = ra_scilab('ra_prop_get_by_name', eval_handle, name);
