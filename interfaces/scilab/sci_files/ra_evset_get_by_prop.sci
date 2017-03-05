function [evset_handle] = ra_evset_get_by_prop(prop_handle)
// Returns the evaluation-set-handle which is associated
// the event-property 'evprop-handle'.
//   Input parameters:
//     prop_handle
// 
//   Output parameters:
//     evset_handle

	[evset_handle] = ra_scilab('ra_evset_get_by_prop', prop_handle);
