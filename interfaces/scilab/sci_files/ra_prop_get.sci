function [value] = ra_prop_get(class_handle, id)
// 
//     Return the event-propertiy with the id 'id'.
//     Input parameters:
//     class_handle
//     id
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_prop_get', class_handle, id);
