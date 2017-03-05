echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
[v, n, d]=ra_meas_info_get(meas, 'num_sessions')
[v, n, d]=ra_meas_info_get(meas, 'max_samplerate')
num=ra_meas_info_get(meas, 'num_obj_infos')
for i=0:(num-1)
    [v,n,d]=ra_info_get_by_idx(meas, 'meas', i)
endfor
ra_meas_close(meas);
ra_lib_close(ra);
