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
    die "no evaluation in measurement\n";

# get some general infos
my $v = $eval->get_info(info => 'eval_name');
my $eval_name = $v->value();
$v = $eval->get_info(info => 'eval_add_timestamp');
my $eval_add_ts = $v->value();
$v = $eval->get_info(info => 'eval_program');
my $eval_prg = $v->value();
print "evaluation $eval_name was added at $eval_add_ts" .
    " using the program $eval_prg\n\n";


# list event-class
my $cl_all = $eval->get_class();
for my $cl_curr (@$cl_all)
{
    my $name = $cl_curr->get_info(info => 'class_name');
    my $num_events = $cl_curr->get_info(info => 'class_num_events');

    print "event-class " . $name->value() . " with " . $num_events->value() . " events\n";

    # list event properties
    my $props = $cl_curr->get_prop_all();
    for my $prop (@$props)
    {
        my $name = $prop->get_info(info => 'prop_name');
        my $desc = $prop->get_info(info => 'prop_desc');

        print "  " . $name->value() . "\n";
	print "    " . $desc->value() . "\n";
    }
}

exit 0;
#]]>
