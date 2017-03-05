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

# get default evaluation
eva = meas.get_def_eval()
if not eva:
    print "no evaluation in measurement"
    sys.exit()

# get event-properties for RR-intervals and position of QRS-complexs
prop_rri = eva.get_evprop_by_name('rri')
if not prop_rri:
    print "no event-property 'rri' in the evaluation"
    sys.exit()

prop_qrs_pos = eva.get_evprop_by_name('qrs-pos')
if not prop_qrs_pos:
    print "no event-property 'qrs-pos' in the evaluation"
    sys.exit()

# get values for all RR-intervals and QRS-complexes
rri = prop_rri.get_events()
qrs_pos_ref = prop_qrs_pos.get_events()

# now do something with the RR-intervals and QRS-complex-positions

#]]>
