echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
eva=ra_eval_get_default(meas)

samplerate=ra_meas_info_get(meas, 'max_samplerate')

cl = ra_class_get(eva, 'heartbeat')
num_hb = length(cl)
for i = 1:num_hb
  ev = ra_class_get_events(cl(i), -1, -1, 0, 1);
  num_ev = length(ev)

  prop_rri = ra_prop_get(cl(i), 'rri')
  prop_qrs_pos = ra_prop_get(cl(i), 'qrs-pos')
  
  rri = [];
  qrs_pos = [];
  for j = 1:num_ev
    r = ra_prop_get_value(prop_rri, ev(j), -1);
    p = ra_prop_get_value(prop_qrs_pos, ev(j), -1);

    rri = [rri r];
    qrs_pos = [qrs_pos p];
  endfor

  %% To get all property values within one function call
  %% use ra_prop_get_value_all():
  % [ev_ids, chs, rri] = ra_prop_get_value_all(prop_rri);
  % [ev_ids, chs, qrs_pos] = ra_prop_get_value_all(prop_qrs_pos);
  %
  %% !!! This function returns also the event-id's and channel
  %% !!! numbers the value belongs to. The order of returned values
  %% !!! is not guaranteed to be in chronological order.

  whos rri
  whos qrs_pos

  figure();
  x=(qrs_pos/samplerate) / 60;
  plot(x, rri);
endfor

ra_meas_close(meas);
ra_lib_close(ra);
