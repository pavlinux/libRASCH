function [value, name, desc] = ra_dev_get_info(rec_handle, dev_num, info_name)
// Returns the information 'info_name' about a
// measurement-device. The name and a short description is also returned. See
// XXX for available informations.
//   Input parameters:
//     rec_handle
//     dev_num
//     info_name
// 
//   Output parameters:
//     value
//     name
//     desc

	[value, name, desc] = ra_scilab('ra_dev_get_info', rec_handle, dev_num, info_name);
