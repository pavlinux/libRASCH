#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement and get default evaluation
my $meas = $ra->open_meas($ARGV[0], '', 0) or
    die "can't open measurement $ARGV[0]\n";

# get plugin-handle for hrv-plugin
my $pl = $ra->get_plugin_by_name('hrv') or
    die "can't find plugin 'hrv'\n";

# calculate hrv-values using the hrv-plugin
my $proc = $pl->get_process($meas) or
    die "can't initialize processing\n";
my $results = $proc->process();
# $results is a reference to an array; each array-element contains
# another array with three elements (value, name, description)
for (@$results)
{
    print $_->name() . ' (' . $_->desc() . ') = ' .
	$_->value() . "\n";
}

exit 0;
#]]>
