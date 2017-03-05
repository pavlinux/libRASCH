echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
eva=ra_eval_get_default(meas)
ra_eval_info_get(eva, 'eval_name')
ra_eval_info_get(eva, 'eval_add_timestamp')
ra_eval_info_get(eva, 'eval_program')
cl = ra_class_get(eva, '');
num_cl = length(cl)
for i = 1:num_cl
  name_cl = ra_class_info_get(cl(i), 'class_name')
  desc_cl = ra_prop_info_get(cl(i), 'class_desc')
  num_events = ra_class_info_get(cl(i), 'class_num_events')

  props = ra_prop_get_all(cl(i));
  num_prop = length(props)
  for j = 1:num_prop
    id_ascii = ra_prop_info_get(props(j), 'prop_id_ascii')
    desc_prop = ra_prop_info_get(props(j), 'prop_desc')
  endfor
endfor

ra_meas_close(meas);
ra_lib_close(ra);
