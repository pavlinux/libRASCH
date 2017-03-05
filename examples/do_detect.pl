use strict;
use RASCH;

# initialize libRASCH
my $ra = new RASCH or die "Can not initialize libRASCH\n";
check_error($ra);

# open measurement
my $meas = $ra->open_meas($ARGV[0], 0) or check_error($ra);
my $rec = $meas->get_first_session_rec(0) or check_error($ra);

# get plugin-handle for simple-detect-plugin
my $pl = $ra->get_plugin_by_name('detect-simple') or check_error($ra);

	if ((pl = ra_plugin_by_name(ra, "detect-simple", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_get_proc_handle(pl);
		pi->plugin = pl;
		pi->mh = mh;
		pi->rh = ra_get_first_session_rec(mh, 0);
		pi->save_in_eval = 1;
		pi->private_data = &ch_list;
		pi->callback = detect_callback;

		ra_do_processing(pi);
		ra_free_proc_handle(pi);
	}




# calculate hrv-values using the hrv-plugin
my $proc = $pl->init_process(rec => $rec) or die "can't initialize processing\n";
my $results = $proc->process();
# $results is a reference to an array; each array-element contains
# another array with three elements (value, name, description)
for (@$results)
{
    print "$_->[1] ($_->[2]) = $_->[0]\n";
}

exit 0;

#################### functions

sub check_error
{
    my $ra = shift;
    
    my ($err_num, $err_text) = $ra->get_error();
    if ($err_num != 1)
    {
	my $text = "error # $err_num occured:\n $err_text\n";
	    die $text;
    }
} # sub check_error()
