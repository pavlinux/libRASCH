echo on
ra=ra_lib_init
[err_num, err_text]=ra_lib_get_error(ra)
[value, name, desc]=ra_lib_info_get(ra, 'num_plugins')
meas=ra_meas_find(ra, './database')
ra_lib_close(ra);
