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

# get root recording
rec = meas.get_first_rec(0)
if not rec:
    print "can't get root recording"
    sys.exit()

# get some infos about recording
num_dev = rec.get_info(info='rec_num_devices')
num_ch = rec.get_info(info='rec_num_channel')
rec_name = rec.get_info(info='rec_name')
rec_date = rec.get_info(info='rec_date')
print "measurement", rec_name.value()
print "recorded at", rec_date.value()
print "#devices =", num_dev.value()
print "#channels =", num_ch.value()
print

# print name for every device
print "infos about the recording devices used:"
if num_dev.value() > 0:
    for i in range(num_dev.value()):
        name = rec.get_info(dev=i, info='dev_hw_name')
        print "  device #%d: %s" % (i, name.value())
print ""
	
# print name for every channel
print "infos about the channels:";
if num_ch.value() > 0:
    for i in range(num_ch.value()):
        name = rec.get_info(ch=i, info='ch_name')
        unit = rec.get_info(ch=i, info='ch_unit')
        print "  channel #%d: %s [%s]" % (i, name.value(), unit.value())
print "";
    
#]]>
