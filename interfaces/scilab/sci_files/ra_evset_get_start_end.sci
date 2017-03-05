function [start_pos, end_pos] = ra_evset_get_start_end(evset_handle, start, num)
// Returns 'num' start- and end-postions of continues events
// starting with event 'start', from event-set 'evset_handle' and
// evaluation 'eval_handle'.
//   Input parameters:
//     evset_handle
//     start
//     num
// 
//   Output parameters:
//     start_pos
//     end_pos


if (start == -1)
  start = 0;
end
if (num == -1)
  t = ra_evset_get_info(evset_handle, 'eval_set_num_events');
  num = double(t);
end


	[start_pos, end_pos] = ra_scilab('ra_evset_get_start_end', evset_handle, start, num);
