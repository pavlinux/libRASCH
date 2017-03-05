function [prop_handle] = ra_prop_get_by_num(eval_handle, index)
// Returns the event-property-handle number 'index'.
//   Input parameters:
//     eval_handle
//     index
// 
//   Output parameters:
//     prop_handle

	[prop_handle] = ra_scilab('ra_prop_get_by_num', eval_handle, index);
