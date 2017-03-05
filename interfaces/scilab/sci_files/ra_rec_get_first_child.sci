function [rec_handle] = ra_rec_get_first_child(rec_handle)
// Returns the recording-handle which is the first child of
// 'rec_handle'. 
//   Input parameters:
//     rec_handle
// 
//   Output parameters:
//     rec_handle

	[rec_handle] = ra_scilab('ra_rec_get_first_child', rec_handle);
