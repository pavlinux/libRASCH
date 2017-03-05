function [value, name, desc] = ra_class_info_get(class_handle, info_name)
// Returns the information 'info_name' about an
// evaluation-class. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     class_handle
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_class_info_get', class_handle, info_name);
