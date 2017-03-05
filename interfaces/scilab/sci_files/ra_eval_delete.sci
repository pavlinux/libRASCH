function [ok] = ra_eval_delete(eval_handle)
// Delete the evaluation 'eval_handle'.
//     Input parameters:
//     eval_handle
// 
//   Output parameters:
//     ok

	[ok] = ra_scilab('ra_eval_delete', eval_handle);
