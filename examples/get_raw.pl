#<![CDATA[
use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "error initializing libRASCH\n";

# open measurement
my $meas = $ra->open_meas($ARGV[0], 0) or
    die "can't open measurement $ARGV[0]\n";

# get root recording
my $rec = $meas->get_first_session_rec(0) or
    die "can't get root recording\n";

# get first 10000 samples for each channel
my ($num_ch) = $rec->get_info(info => 'rec_num_channel');
for (my $i = 0; $i < $num_ch; $i++)
{
    my $data_ref = $rec->get_raw($i, 0, 10000);
    for (@$data_ref)
    {
	; # do something with every sample
    }
}

exit 0;
#]]>
