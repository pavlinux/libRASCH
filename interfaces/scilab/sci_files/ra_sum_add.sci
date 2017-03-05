function [sum_handle] = ra_sum_add(class_handle, id, name, desc, num_dim, dim_name, dim_unit)
// Returns the handle of the added event-summary.
//     Input parameters:
//     class_handle
//     id
//     name
//     desc
//     num_dim
//     dim_name
//     dim_unit
// 
//   Output parameters:
//     sum_handle

	[sum_handle] = ra_scilab('ra_sum_add', class_handle, id, name, desc, num_dim, dim_name, dim_unit);
