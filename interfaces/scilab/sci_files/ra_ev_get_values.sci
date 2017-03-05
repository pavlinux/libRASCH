function [values] = ra_ev_get_values(prop_handle, start, num)
// Returns 'num' event-values starting with 'start' from the
// evaluation 'eval_handle' and the event-property 'prop_handle'.
//   Input parameters:
//     prop_handle
//     start
//     num
// 
//   Output parameters:
//     values


if (start == -1)
  start = 0;
end
if (num == -1)
  seth = ra_evset_get_by_prop(prop_handle);
  t = ra_evset_get_info(seth, 'eval_set_num_events');
  num = double(t);
end


	[values] = ra_scilab('ra_ev_get_values', prop_handle, start, num);
