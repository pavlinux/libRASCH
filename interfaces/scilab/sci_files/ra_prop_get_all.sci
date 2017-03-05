function [value] = ra_prop_get_all(class_handle)
// 
//     Returns all event-properties within event-class 'class_handle'.
//     Input parameters:
//     class_handle
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_prop_get_all', class_handle);
