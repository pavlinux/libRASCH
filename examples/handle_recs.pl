#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement
my $meas = $ra->open_meas($ARGV[0], '', 0) or
    die "can't open measurement $ARGV[0]\n";

# get root recording
my $rec = $meas->get_first_rec(0) or
    die "can't get root recording\n";

# get some infos about recording
my $v = $rec->get_info(info => 'rec_num_devices');
my $num_dev = $v->value();
$v = $rec->get_info(info => 'rec_num_channel');
my $num_ch = $v->value();
$v = $rec->get_info(info => 'rec_name');
my $rec_name = $v->value();
$v = $rec->get_info(info => 'rec_date');
my $rec_date = $v->value;
print "measurement $rec_name\nrecorded at $rec_date\n" .
    "#devices=$num_dev\n#channels=$num_ch\n\n";

# print name for every device
print "infos about the recording devices used:\n";
for (my $i = 0; $i < $num_dev; $i++)
{
    $v = $rec->get_info(dev => $i, info => 'dev_hw_name');
    my $name = $v->value();
    print "  device #$i: $name\n";
}
print "\n";
	
# print name for every channel
print "infos about the channels:\n";
for (my $i = 0; $i < $num_ch; $i++)
{
    $v = $rec->get_info(ch => $i, info => 'ch_name');
    my $name = $v->value();
    $v = $rec->get_info(ch => $i, info => 'ch_unit');
    my $unit = $v->value();
    print "  channel #$i: $name [$unit]\n";
}
print "\n";

exit 0;
#]]>
