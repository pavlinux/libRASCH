function [data] = ra_raw_get_unit(rec_handle, channel, start_sample, num_samples)
// Returns 'num_samples' raw-data starting with sample
// 'start_sample' from recording 'rec_handle' and channel 'ch'. The values
// returned are scaled so the values are 'unit'-values
//   Input parameters:
//     rec_handle
//     channel
//     start_sample
//     num_samples
// 
//   Output parameters:
//     data

	[data] = ra_scilab('ra_raw_get_unit', rec_handle, channel, start_sample, num_samples);
