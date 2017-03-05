function [value] = ra_eval_attribute_list(handle)
// 
//     Get the list of available attributes for evaluation related handle 'handle'.
//     Input parameters:
//     handle
// 
//   Output parameters:
//     value

	[value] = ra_scilab('ra_eval_attribute_list', handle);
