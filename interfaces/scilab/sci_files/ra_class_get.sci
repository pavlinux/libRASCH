function [value] = ra_class_get(eval_handle, id)
// 
//     Returns all event-classes with the id 'id'.
//     Input parameters:
//     eval_handle
//     id
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_class_get', eval_handle, id);
