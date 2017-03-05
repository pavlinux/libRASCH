function [evset_handle] = ra_evset_get_by_name(eval_handle, name)
// Returns the evaluation-set-handle with the name 'name'.
//   Input parameters:
//     eval_handle
//     name
// 
//   Output parameters:
//     evset_handle

	[evset_handle] = ra_scilab('ra_evset_get_by_name', eval_handle, name);
