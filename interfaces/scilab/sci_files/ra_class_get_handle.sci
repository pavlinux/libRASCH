function [class_handle] = ra_class_get_handle(handle)
// Returns the class-handle the property or summary handle 'handle' belongs to.
//     Input parameters:
//     handle
// 
//   Output parameters:
//     class_handle

	[class_handle] = ra_scilab('ra_class_get_handle', handle);
