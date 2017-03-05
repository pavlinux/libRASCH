function [OK] = ra_sum_add_ch(sum_handle, ch, fid_offset)
// Add a channel to the event-summary 'sum_handle'.
//     Input parameters:
//     sum_handle
//     ch
//     fid_offset
// 
//   Output parameters:
//     OK

	[OK] = ra_scilab('ra_sum_add_ch', sum_handle, ch, fid_offset);
