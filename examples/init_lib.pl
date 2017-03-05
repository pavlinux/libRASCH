#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";
my ($err_num, $err_text) = $ra->get_error ();
if ($err_num != 1)
{
    print "while initializing libRASCH, error # $err_num " .
        "occured:\n $err_text\n";
    exit -1;
}

# get some infos
my $value = $ra->get_info(info =>'num_plugins');
if ($value->is_ok())
{
    print $value->name() . " (" . $value->desc() . "): " . $value->value() . "\n";
}

# find all measurements in a directory
my $meas = $ra->find_meas($ARGV[0]);
print "measurements found in $ARGV[0]:\n";
my $cnt = 1;
for (@$meas)
{
    print "$cnt: " . $_->filename() . "\n";
    $cnt++;
}

# ra_close() will be called when $ra is being destroyed

exit 0;
#]]>
