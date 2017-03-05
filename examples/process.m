%echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
eva=ra_eval_get_default(meas)
pl=ra_plugin_get_by_name(ra, 'hrv', 0)
proc=ra_proc_get(meas, pl)
ra_proc_do(proc)
[v, n, d]=ra_proc_get_result(proc, pl);
num=length(v)
for i=1:num-3
  if strcmp(typeinfo(v{i}), '<unknown type>')
    continue;
  endif
  if !strcmp(typeinfo(v{i}), 'scalar')
    printf("%d: %s (%s) = [not a scalar value]\n", i, n(i,:), d(i,:));
  else 
    printf("%d: %s (%s) = %f\n", i, n(i,:), d(i,:), v{i});
  endif
endfor

ra_meas_close(meas);
ra_lib_close(ra);
