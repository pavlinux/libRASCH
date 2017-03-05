#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement
my $meas = $ra->open_meas($ARGV[0], '', 0) or
    die "can't open measurement $ARGV[0]\n";

# get default evaluation
my $eval = $meas->get_default_eval() or
    die "no evaluation in the measurement\n";

my $cl_all = $eval->get_class();
for (@$cl_all)
{
    my $curr_cl = $_;

    my $name = $curr_cl->get_info(info => 'class_name');
    my $num_events = $curr_cl->get_info(info => 'class_num_events');
    print "event-class " . $name->value() . " with " . $num_events->value() . " events\n";

    my $prop_all = $curr_cl->get_prop_all();
    for (@$prop_all)
    {
	my $curr_prop = $_;
	my ($event_ids, $chs, $values) = $curr_prop->get_value_all();

        my $name = $curr_prop->get_info(info => 'prop_name')->value();
        my $desc = $curr_prop->get_info(info => 'prop_desc')->value();

	my $cnt = 0;
	for (@$event_ids)
	{
	    print $name . '-event ' . $event_ids->[$cnt] . ': ch=' . $chs->[$cnt] . ' value=' . $values->[$cnt] . "\n";
	    $cnt++;
	}
    }
}

exit 0;
#]]>
