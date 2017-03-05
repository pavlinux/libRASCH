echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
rec=ra_rec_get_first(meas, 0)
num_ch=ra_rec_info_get(rec, 'rec_num_channel')
rec_name=ra_rec_info_get(rec, 'rec_name')
rec_date=ra_rec_info_get(rec, 'rec_date')
for i=0:(num_ch-1)
    name=ra_ch_info_get(rec, i, 'ch_name')
    unit=ra_ch_info_get(rec, i, 'ch_unit')
endfor
ra_meas_close(meas);
ra_lib_close(ra);
