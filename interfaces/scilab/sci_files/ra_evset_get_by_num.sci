function [evset_handle] = ra_evset_get_by_num(eval_handle, number)
// Returns the evalution-set-handle number 'number'.
//   Input parameters:
//     eval_handle
//     number
// 
//   Output parameters:
//     evset_handle

	[evset_handle] = ra_scilab('ra_evset_get_by_num', eval_handle, number);
