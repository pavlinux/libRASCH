#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement
my $meas = $ra->open_meas($ARGV[0], '', 0) or
    die "can't open measurement $ARGV[0]\n";

# get some infos
my $v = $meas->get_info(info => 'num_sessions');
print $v->name() . ' (' . $v->desc() . '): ' . $v->value() ."\n" if (defined($v));
$v = $meas->get_info(info => 'max_samplerate');
print $v->name() . ' (' . $v->desc() . '): ' . $v->value() ."\n" if (defined($v));

# get all measurement-object infos
$v = $meas->get_info(info => 'num_obj_infos');
my $num = $v->value();
for (my $i = 0; $i < $num; $i++)
{
    $v = $meas->get_info_idx(index => $i);
    print $v->name() . ' (' . $v->desc() . '): ' . $v->value() ."\n" if (defined($v));
}

exit 0;
#]]>
