#<![CDATA[
import sys
from RASCH import *

# initialize libRASCH
ra = RASCH()
if not ra:
    print "can't initialize libRASCH"
    sys.exit()

# open measurement and get default evaluation
meas = ra.open_meas(sys.argv[1], '', 0)
if not meas:
    print "can't open measurement", sys.argv[1]
    sys.exit()

# get plugin-handle for hrv-plugin
pl = ra.get_plugin_by_name('hrv')
if not pl:
    print "can't find plugin 'hrv'"
    sys.exit();

# calculate hrv-values using the hrv-plugin
proc = pl.get_process(meas)
if not proc:
    print "can't initialize processing"
    sys.exit()
    
results = proc.process()
for item in results:
    if item.is_ok():
        print item.name(), "("+item.desc()+") =", item.value()    
#]]>
