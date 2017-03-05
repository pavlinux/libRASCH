echo on
ra=ra_lib_init
meas=ra_meas_open(ra, './database/100s.hea', '', 0)
rec=ra_rec_get_first(meas, 0)
num_ch=ra_rec_info_get(rec, 'rec_num_channel')
ch_all=[];
for i=0:(num_ch-1)
  ch=ra_raw_get_unit(rec, i, 0, 10000);
  ch_all=[ch_all ch'];
endfor
whos ch_all
samplerate=ra_ch_info_get(rec, 0, 'ch_samplerate')
x=0:9999;
x = x / samplerate;

figure();
for i=1:num_ch
  subplot(num_ch,1,i)
  plot(x,ch_all(:,i))
endfor

ra_meas_close(meas);
ra_lib_close(ra);
