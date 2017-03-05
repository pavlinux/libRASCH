#<![CDATA[
import sys
from RASCH import *

# initialize libRASCH
ra = RASCH()
if not ra:
    print "can't initialize libRASCH"
    sys.exit()
[err_num, err_text] = ra.get_error()
if err_num != 1:
    print "while initializing libRASCH, error #%d occured:\n " \
          "%s\n" % err_num, err_text
    sys.exit()
    
# get some infos
value = ra.get_info(info='num_plugins')
if (value.is_ok()):
    print value.name(), "("+value.desc()+"):", value.value()

# find all measurements in a directory
meas = ra.find_meas(sys.argv[1])
print "measurements found in " + sys.argv[1] + ":\n"
cnt = 1
for item in meas:
    print "%d: %s" % (cnt, item.filename())
    cnt = cnt + 1

#]]>
