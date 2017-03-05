#<![CDATA[
import sys
from RASCH import *

# initialize libRASCH
ra = RASCH()
if not ra:
    print "can't initialize libRASCH"
    sys.exit()

# open measurement
meas = ra.open_meas(sys.argv[1], '', 0)
if not meas:
    print "can't open measurement", sys.argv[1]
    sys.exit()

# get some infos
value = meas.get_info(info='num_sessions')
if value.is_ok():
    print value.name(), "("+value.desc()+"):", value.value()
value = meas.get_info(info='max_samplerate')
if value.is_ok():
    print value.name(), "("+value.desc()+"):", value.value()

# get all measurement-object infos
num = meas.get_info(info='num_obj_infos')
for i in range(num.value()):
    value = meas.get_info_idx(index=i)
    if value.is_ok():
        print value.name(), "("+value.desc()+"):", value.value()

#]]>
