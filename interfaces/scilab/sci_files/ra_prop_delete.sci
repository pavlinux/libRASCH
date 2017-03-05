function [ok] = ra_prop_delete(prop_handle)
// Delete the event-property given by 'prop_handle'.
//     Input parameters:
//     prop_handle
// 
//   Output parameters:
//     ok

	[ok] = ra_scilab('ra_prop_delete', prop_handle);
