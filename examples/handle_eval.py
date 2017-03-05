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

# get default evaluation
eva = meas.get_default_eval()
if not eva:
    print "no evaluation in measurement"
    sys.exit()

# get some general infos
eval_name = eva.get_info(info='eval_name')
eval_add_ts = eva.get_info(info='eval_add_timestamp')
eval_prg = eva.get_info(info='eval_program')
print "evaluation", eval_name.value(), "was added at", eval_add_ts.value(), \
      "using the program", eval_prg.value(), "\n"

# list event-class
cl = eva.get_class()
for cl_curr in cl:
    name = cl_curr.get_info(info='class_name')
    num_events = cl_curr.get_info(info='class_num_events')

    print "event-class", name.value(), "with",
    print num_events.value(),
    print "events"

    # list event properties
    props = cl_curr.get_prop_all()
    for elem in props:
        name = elem.get_info(info='prop_name')
        desc = elem.get_info(info='prop_desc')

        print " ", name.value()
        print "   ", desc.value()

#]]>
