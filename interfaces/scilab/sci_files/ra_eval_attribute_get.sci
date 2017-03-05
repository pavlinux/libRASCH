function [value] = ra_eval_attribute_get(handle, attrib_name)
// 
//     Get the value of the attribute 'attrib_name'.
//     Input parameters:
//     handle
//     attrib_name
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_eval_attribute_get', handle, attrib_name);
