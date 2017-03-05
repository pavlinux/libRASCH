#<![CDATA[
import sys
from RASCH import *

# initialize libRASCH
ra = RASCH()
if not ra:
    print "can't initialize libRASCH"
    sys.exit()

# open measurement
meas = ra.open_meas(sys.argv[1], 0)
if not meas:
    print "can't open measurement", sys.argv[1]
    sys.exit()

# get root recording
rec = meas.get_first_session_rec(0)
if not rec:
    print "can't get root recording"
    sys.exit()

# get first 10000 samples for each channel
[num_ch, n, d] = rec.get_info(info='rec_num_channel')
if num_ch > 0:
    for i in range(num_ch):
        data = rec.get_raw(i, 0, 10000)
        for elem in data:
            elem # do something with every sample

#]]>
