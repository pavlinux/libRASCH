function [value] = ra_sum_get(class_handle, id)
// 
//     Returns all event-summaries with the id 'id'.
//     Input parameters:
//     class_handle
//     id
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_sum_get', class_handle, id);
