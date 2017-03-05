#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement
my $meas = $ra->open_meas($ARGV[0], 0) or
    die "can't open measurement $ARGV[0]\n";

# get default evaluation
my $eval = $meas->get_default_eval() or
    die "no evaluation in the measurement\n";

my $cl = $eval->get_class('heartbeat');
# we are only interested in the first 'heartbeat' event-class
# in this example
my $cl_use = $cl->[0];

# get event-properties for RR-intervals and position of QRS-complex
my $prop_rri = $cl_use->get_prop('rri') or
    die "no event-property 'rri' in the evaluation\n";
my $prop_qrs_pos = $cl_use->get_prop('qrs-pos') or
    die "no event-property 'qrs-pos' in the evaluation\n";

# get values for all RR-intervals and QRS-complexes
my $rri_ref = $prop_rri->get_events();
my $qrs_pos_ref = $prop_qrs_pos->get_events();

# now do something with the RR-intervals and QRS-complex-positions

exit 0;
#]]>
