#! /usr/bin/perl

#
# Program to get QT values for the Computers in Cardiology Challenge 2006.
#
# Copyright 2006 by Raphael Schneider (rasch@librasch.org)
#
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
# This Perl script needs libRASCH version 0.8.0pre2 or greater from www.librasch.org
# and the Perl API bindings have to be installed also (see INSTALL file coming
# with the libRASCH distribution).
#
# Usage:
#
#   perl get_qt_cinc_2006.pl --template=templ-file --file=out-file
#                            [--detect] [--ch=X] [--check-morphology]
#                            [--skip-powerline] <ecg-dir(s)>
#
#   with --template=templ-file  selecting the template of the entry-form
#        --file=out-file        selecting the name of the final entry-form
#        --detect               if given, a beat detection is performed, if not,
#                               the beat information (position and annotation) which
#                               is available is used
#        --ch=X                 do the QT measurement using channel 'X'
#                               (channel-numbers start with 0 -> lead I is 0, lead II is 1, ...)
#        --check-morphology     if given, the morphologies of the QRS complexes and T-waves
#                               are forced to be the same for all beats (e.g. when the Q-wave
#                               on some beats is too small to be detected but seen on the
#                               majority of the beats, the threshold used to detect Q-waves
#                               is reduced so if small Q-waves will be detected)
#        --skip-powerline       if given, the powerline-noise filter is NOT applied to
#                               the raw ecg data
#        <ecg-dir(s)>           the directory(s) with the ECGs to process
#
# Example:
#
#   perl get_qt_cinc_2006.pl --detect --ch=1
#                            --template=template_div3.txt --file=results_div3.txt
#                            ./patient*
#   
#
# Version 0.0: rasch@librasch.org (2006-04-19)
#   first verision
# Version 1.0: rasch@librasch.org (2006-09-11)
#   final changes before sending it to PhysioNet
# Version 1.1: rasch@librasch.org (2006-09-12)
#   added option to select algorithm to get T-wave end
#

use strict;
use warnings;
$|++;

use RASCH;
use Getopt::Long;

my $debug = 1;


########## get and check options ##########
my $do_detect;
my $ch = 1;  # ch-1 is lead II
my $check_morphology;
my $method = 0;
my $outfile;  # default is print to stdout
my $template_file = './template.txt';
my $skip_powerline_filter;
my $t_end_algorithm = 0;  # default (=0) is line method
my $export_more;
GetOptions("detect" => \$do_detect,
	   "ch=s" => \$ch,
	   "check-morphology" => \$check_morphology,
	   "method=s" => \$method,
	   "file=s" => \$outfile,
	   "template=s" => \$template_file,
	   "skip-powerline" => \$skip_powerline_filter,
	   "t-algorithm=s" => \$t_end_algorithm,
	   "noise" => \$export_more);


# if no arguments left -> no files/dirs given to process
exit 0 if ($#ARGV < 0);

# check if file-/dir-names are given on stdin
my @files_use;
if ($ARGV[0] eq '-')
{
    while (<>)
    {
	my $fn = $_;
	chomp($fn);
	push(@files_use, $fn);
    }
}
else
{
    @files_use = @ARGV;
}

########## start processing the files/dirs ##########
my $ra = new RASCH or die "can't open libRASCH\n";

my $meas_cnt = 0;
my %results = ();
for my $curr (@files_use)
{
    my $meas = $ra->open_meas($curr);
    if (defined($meas))
    {
	$meas_cnt++;
	print STDERR "process measurement $curr (#$meas_cnt)...";
	process_meas($ra, $meas, $do_detect, $ch, $check_morphology,
		     $method, $skip_powerline_filter, , $t_end_algorithm, \%results);
	print STDERR "\n";
    }
    else
    {
	# open was not sucessful, maybe it is a directory
	my @sigs = @{$ra->find_meas($curr)};
	if ($#sigs < 0)
	{
	    print STDERR "can't handle file/dir $curr\n";
	    next;
	}
	for my $curr2 (@sigs)
	{
	    my $fn = $curr2->filename();
	    $meas_cnt++;
	    print STDERR "process measurement $fn in directory $curr (#$meas_cnt)...";
	    $meas = $ra->open_meas($fn);
	    if (not defined($meas))
	    {
		print STDERR "\nError opening $fn. This should not happen\n";
		next;
	    }
	    process_meas($ra, $meas, $do_detect, $ch, $check_morphology,
			 $method, $skip_powerline_filter, $t_end_algorithm, \%results);
	    print STDERR "\n";
	}
    }
}


########## write results ##########
write_results($template_file, $outfile, $export_more, \%results);

exit 0;



######################### sub-routines #########################


sub process_meas
{
    my ($ra, $meas, $do_detect, $ch, $check_morph,
	$method, $skip_powerline_filter, $t_end_algorithm, $results) = @_;

    my $clh = undef;
    if ($do_detect)
    {
	$clh = do_detect($ra, $meas, $ch, $check_morph, $skip_powerline_filter, $t_end_algorithm);
	return undef if (not defined($clh));
    }
    else
    {
	my $eva = $meas->get_default_eval();
	if (not defined($eva))
	{
	    print STDERR "nok";
	    return undef;
	}
	my $all_clh = $eva->get_class('heartbeat');
	$clh = $all_clh->[0];
	if ((not defined($clh)) || ($clh == 0))
	{
	    print STDERR "nok";
	    return undef;
	}
    }

    my $rec = $meas->get_first_rec();
    my ($value, $name, $desc) = $rec->get_info(info => 'rec_name');
    my $rec_name = $value->value();

    ($value, $name, $desc) = $rec->get_info(info => 'ch_samplerate', ch => $ch);
    my $samplerate = $value->value();

    my $res;
    if ($method == 0)
    {
	$res = qt_method0($clh, $ch);
    }
    elsif ($method == 1)
    {
	$res = qt_method1($clh, $ch);
    }
    elsif ($method == 2)
    {
	$res = qt_method2($clh, $ch);
    }
    else
    {
	print STDERR "Unknown method to get QT times. Please select a known one (0-2).\n";
	exit -1;
    }
    $res->[2] = $samplerate;

    $results->{$rec_name} = $res;

    return $res;
} # sub process_meas()



#################### beat detection code ####################
sub do_detect
{
    my ($raq, $meas, $ch, $check_morph, $skip_powerline_filter, $t_end_algorithm) = @_;

    my $eva = $meas->add_eval("get_qt.pl", "evaluation for CinC Challenge 2006");
    if (not defined($eva))
    {
	print STDERR "Error creating evaluation";
	return 0;
    }
    
    do_beat_detect($ra, $meas, $eva, $ch, $skip_powerline_filter, $t_end_algorithm);
    my $all_clh = $eva->get_class('heartbeat');
    my $clh = $all_clh->[0];
    if ((not defined($clh)) || ($clh == 0))
    {
	print STDERR "nok";
	return undef;
    }

    # get most used morphologies for qrs and t
    my ($qrs_morph_use, $t_morph_use) = get_max_morph($clh);

    # now do beat-detection again forcing to use the
    # maxed used morphologies
    do_beat_detect($ra, $meas, $eva, $ch, $skip_powerline_filter, $t_end_algorithm, $clh, $qrs_morph_use, $t_morph_use);
    $all_clh = $eva->get_class('heartbeat');
    $clh = $all_clh->[0];
    if ((not defined($clh)) || ($clh == 0))
    {
	print STDERR "nok";
	return undef;
    }

    do_fiducial($ra, $meas, $clh);
    do_template($ra, $meas, $clh);
    if ($check_morph)
    {
	do_morphology_check($ra, $meas, $eva, $clh, $skip_powerline_filter, $t_end_algorithm);
    }
    do_ecg($ra, $meas, $clh);

    $meas->save_eval();

    return $clh;
} # sub do_detect()


sub do_beat_detect
{
    my $ra = shift;
    my $meas = shift;
    my $eva = shift;
    my $ch = shift;
    my $skip_powerline_filter = shift;
    my $t_end_algorithm = shift;
    my $clh = (defined(@_) ? shift : undef);
    my $qrs_morph_force = (defined(@_) ? shift : undef);
    my $t_morph_force = (defined(@_) ? shift : undef);

    if (defined($clh))
    {
	$clh->delete();
    }

    my $pl = $ra->get_plugin_by_name("detect-ecg", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'detect-ecg' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas->get_handle());
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    my $ret = $proc->set_option('save_in_eval', 'l', 1);
    $ret = $proc->set_option('do_interpolation', 'l', 1);
    $ret = $proc->set_option('eh', 'h', $eva->get_handle());
    $ret = $proc->set_option('filter_powerline_noise', 'l', ($skip_powerline_filter ? 0 : 1));
    $ret = $proc->set_option('t_wave_algorithm', 'l', $t_end_algorithm);
    $ret = $proc->set_option('force_qrs_type', 'l', $qrs_morph_force) if (defined($qrs_morph_force));
    $ret = $proc->set_option('force_t_type', 'l', $t_morph_force) if (defined($t_morph_force));
    
    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    if ($ch == -1)
    {
	for (my $i = 0; $i < $num_ch; $i++)
	{
	    $chs[$i] = $i;
	}
    }
    else
    {
	$chs[0] = $ch;
    }
    $ret = $proc->set_option('ch', 'pl', \@chs);

    my $results = $proc->process();

    return 0;
} # sub do_beat_detect()


sub get_max_morph
{
    my ($clh) = @_;

    my $events = $clh->get_events();

    # get morphology types
    my $prop_qrs_morph = $clh->get_prop('ecg-qrs-type');
    if (not defined($prop_qrs_morph))
    {
	print STDERR "no 'ecg-qrs-type' event-property in the event-class";
	return -1;
    }
    my $prop_t_morph = $clh->get_prop('ecg-t-type');
    if (not defined($prop_t_morph))
    {
	print STDERR "no 'ecg-t-type' event-property in the event-class";
	return -1;
    }

    my %qrs_morph = ();
    my %t_morph = ();
    my $cnt_used = 0;
    for (@$events)
    {
	my $qrs_type = $prop_qrs_morph->get_value($_, $ch);
	next if ($qrs_type <= 0);
	my $t_type = $prop_t_morph->get_value($_, $ch);
	next if ($t_type <= 0);

	$qrs_morph{$qrs_type} = 0 if (not defined($qrs_morph{$qrs_type}));
	$qrs_morph{$qrs_type}++;
	$t_morph{$t_type} = 0 if (not defined($t_morph{$t_type}));
	$t_morph{$t_type}++;

	$cnt_used++;
    }

    my $qrs_morph_use;
    my $num = 0;
    for (keys(%qrs_morph))
    {
	if ($num < $qrs_morph{$_})
	{
	    $qrs_morph_use = $_;
	    $num = $qrs_morph{$_};
	}
    }

    my $t_morph_use;
    $num = 0;
    for (keys(%t_morph))
    {
	# if bi-phasic, use only when more than 75% of the beats where found
	# having a bi-phasic T-wave	
	next if (($_ > 100) && (($t_morph{$_}/$cnt_used) < 0.75));

	if ($num < $t_morph{$_})
	{
	    $t_morph_use = $_;
	    $num = $t_morph{$_};
	}
    }

    return ($qrs_morph_use, $t_morph_use);
} # sub get_max_morph()


sub do_fiducial
{
    my ($ra, $meas, $clh) = @_;

    my $pl = $ra->get_plugin_by_name("fiducial-point", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'fiducial-point' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas->get_handle());
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    my $ret = $proc->set_option('use_class', 'l', 1);
    $ret = $proc->set_option('clh', 'h', $clh->get_handle());
    $ret = $proc->set_option('save_in_class', 'l', 1);
    $ret = $proc->set_option('ch', 'l', 0);

    my $results = $proc->process();

    return 0;
} # sub do_fiducial()


sub do_template
{
    my ($ra, $meas, $clh) = @_;

    my $pl = $ra->get_plugin_by_name("template", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'template' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas->get_handle());
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    my $ret = $proc->set_option('use_class', 'l', 1);
    $ret = $proc->set_option('clh', 'h', $clh->get_handle());
    $ret = $proc->set_option('save_in_class', 'l', 1);
    $ret = $proc->set_option('pos_name', 'c', 'qrs-pos');
    $ret = $proc->set_option('templ_name', 'c', 'qrs-template');
    $ret = $proc->set_option('templ_corr', 'c', 'qrs-template-corr');
    $ret = $proc->set_option('accept', 'd', 0.85);
    $ret = $proc->set_option('slope_accept_low', 'd', 0.6);
    $ret = $proc->set_option('slope_accept_high', 'd', 1.4);
    $ret = $proc->set_option('win_before', 'd', 0.15);
    $ret = $proc->set_option('win_after', 'd', 0.15);

    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    for (my $i = 0; $i < $num_ch; $i++)
    {
	$chs[$i] = $i;
    }

    $ret = $proc->set_option('ch', 'pl', \@chs);

    my $results = $proc->process();

    return 0;
} # sub do_template()


sub do_morphology_check
{
    my ($ra, $meas, $eva, $clh, $skip_powerline_filter, $t_end_algorithm) = @_;

    my $pl = $ra->get_plugin_by_name("detect-ecg", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'detect-ecg' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas->get_handle());
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    my $ret = $proc->set_option('save_in_eval', 'l', 1);
    $ret = $proc->set_option('do_interpolation', 'l', 1);
    $ret = $proc->set_option('eh', 'h', $eva->get_handle());
    $ret = $proc->set_option('clh', 'h', $clh->get_handle());
    $ret = $proc->set_option('t_wave_algorithm', 'l', $t_end_algorithm);
    $ret = $proc->set_option('filter_powerline_noise', 'l', ($skip_powerline_filter ? 0 : 1));
    
    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    if ($ch == -1)
    {
	for (my $i = 0; $i < $num_ch; $i++)
	{
	    $chs[$i] = $i;
	}
    }
    else
    {
	$chs[0] = $ch;
    }
    $ret = $proc->set_option('ch', 'pl', \@chs);

#    my $events = get_max_template_events($clh);
    my $events = $clh->get_events();
    $ret = $proc->set_option('events', 'pl', $events);

    # get morphology types
    my $prop_qrs_morph = $clh->get_prop('ecg-qrs-type');
    if (not defined($prop_qrs_morph))
    {
	print STDERR "no 'ecg-qrs-type' event-property in the event-class";
	return -1;
    }
    my $prop_t_morph = $clh->get_prop('ecg-t-type');
    if (not defined($prop_t_morph))
    {
	print STDERR "no 'ecg-t-type' event-property in the event-class";
	return -1;
    }

    my %qrs_morph = ();
    my %t_morph = ();
    for (@$events)
    {
	my $qrs_type = $prop_qrs_morph->get_value($_, $ch);
	next if ($qrs_type <= 0);
	my $t_type = $prop_t_morph->get_value($_, $ch);
	next if ($t_type <= 0);

	$qrs_morph{$qrs_type} = 0 if (not defined($qrs_morph{$qrs_type}));
	$qrs_morph{$qrs_type}++;
	$t_morph{$t_type} = 0 if (not defined($t_morph{$t_type}));
	$t_morph{$t_type}++;
    }

    my $qrs_morph_use;
    my $num = 0;
    for (keys(%qrs_morph))
    {
	if ($num < $qrs_morph{$_})
	{
	    $qrs_morph_use = $_;
	    $num = $qrs_morph{$_};
	}
    }

    my $t_morph_use;
    $num = 0;
    for (keys(%t_morph))
    {
	if ($num < $t_morph{$_})
	{
	    $t_morph_use = $_;
	    $num = $t_morph{$_};
	}
    }

    print STDERR "qrs-force=$qrs_morph_use t-force=$t_morph_use " if ($debug);

    $ret = $proc->set_option('force_qrs_type', 'l', $qrs_morph_use);
    $ret = $proc->set_option('force_t_type', 'l', $t_morph_use);

    my $results = $proc->process();
} # sub do_morphology_check()


sub do_ecg
{
    my ($ra, $meas, $clh) = @_;

    my $pl = $ra->get_plugin_by_name("ecg", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'ecg' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas->get_handle());
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    my $ret = $proc->set_option('use_class', 'l', 1);
    $ret = $proc->set_option('clh', 'h', $clh->get_handle());
    $ret = $proc->set_option('save_in_class', 'l', 1);

    my $results = $proc->process();

    return 0;
} # sub do_ecg()



#################### QT time assessment ####################


#
# (1) select template containing the most beats
# (2) use beat with lowest noise
#
sub qt_method0
{
    my ($clh, $ch) = @_;

    my $prop_noise = $clh->get_prop('ecg-noise');
    if (not defined($prop_noise))
    {
	print STDERR "no 'ecg-noise' event-property in the event-class";
	return undef;
    }
    my $prop_qt = $clh->get_prop('ecg-qt');
    if (not defined($prop_qt))
    {
	print STDERR "no 'ecg-qt' event-property in the event-class";
	return undef;
    }

    my $events = get_max_template_events($clh);

    # get event-id with lowest noise
    my $ev_id_min_noise;
    my $min_noise = 10000000;
    for (@{$events})
    {
	# skip first beat because of possible measurement problems for it
	next if ($_ == 1);

	my $value = $prop_qt->get_value($_, $ch);
	next if ($value == -1);

	$value = $prop_noise->get_value($_, $ch);
	next if ($value == -1);

	if ($value < $min_noise)
	{
	    $ev_id_min_noise = $_;
	    $min_noise = $value;
	}
    }
    return undef if (not defined($ev_id_min_noise));

    print STDERR "min-noise=$min_noise ev-id=$ev_id_min_noise " if ($debug);

    # get QT related values for export and return it
    return get_qt_values($clh, $ev_id_min_noise, $ch);
} # sub qt_method0()


#
# (1) select template containing the most beats
# (2) use beat with median QT value in the template-class
#
sub qt_method1
{
    my ($clh, $ch) = @_;

    my $prop_qt = $clh->get_prop('ecg-qt');
    if (not defined($prop_qt))
    {
	print STDERR "no 'ecg-qt' event-property in the event-class";
	return undef;
    }

    my $events = get_max_template_events($clh);

    # get event-id with median QT value
    my @data = ();
    for (@{$events})
    {
	my $value = $prop_qt->get_value($_, $ch);
	next if ($value == -1);

	my @curr = ($value, $_);
	push(@data, \@curr);
    }

    my @data_sort = sort { $a->[0] <=> $b->[0] } @data;

    my $len = @data_sort;
    my $ev_use = int($len/2);

    my $ev_id = $data_sort[$ev_use]->[1];
    
    # skip first beat because of possible measurement problems for it
    if ($ev_id == 1)
    {
	$ev_use++;
	$ev_id = $data_sort[$ev_use]->[1];
    }
    
    print STDERR "median-qt=$data_sort[$ev_use]->[0] ev-id=$ev_id " if ($debug);

    # get QT related values for export and return it
    return get_qt_values($clh, $ev_id, $ch);    
} # sub qt_method1()


#
# (1) use beat with median QT value over all beats
#
sub qt_method2
{
    my ($clh, $ch) = @_;

    my $prop_qt = $clh->get_prop('ecg-qt');
    if (not defined($prop_qt))
    {
	print STDERR "no 'ecg-qt' event-property in the event-class";
	return undef;
    }

    my $events = $clh->get_events();

    # get event-id with median QT value
    my @data = ();
    for (@{$events})
    {
	my $value = $prop_qt->get_value($_, $ch);
	next if ($value == -1);

	my @curr = ($value, $_);
	push(@data, \@curr);
    }

    my @data_sort = sort { $a->[0] <=> $b->[0] } @data;

    my $len = @data_sort;
    my $ev_use = int($len/2);

    my $ev_id = $data_sort[$len]->[1];

    # skip first beat because of possible measurement problems for it
    if ($ev_id == 1)
    {
	$ev_use++;
	$ev_id = $data_sort[$ev_use]->[1];
    }

    print STDERR "median-qt=$data_sort[$ev_use]->[0] ev-id=$ev_id " if ($debug);

    # get QT related values for export and return it
    return get_qt_values($clh, $ev_id, $ch);    
} # sub qt_method2()



########## QT time assessment helper functions ##########
sub get_max_template_events
{
    my ($clh) = @_;

    my $summaries = $clh->get_sum('template');
    if (not defined($summaries))
    {
	print STDERR "no 'template' summaries in the event-class";
	return undef;
    }

    my $templ = $summaries->[0];
    my $parts = $templ->get_part_all();
    
    # get template (and the event-id's) containing the most beats
    my @events_all = ();
    my $num_ev_max = 0;
    my $ev_idx_max;
    my $cnt = 0;
    for (@{$parts})
    {
	my $ev = $templ->get_part_events($_);
	push(@events_all, $ev);
	if (@{$ev} > $num_ev_max)
	{
	    $num_ev_max = @{$ev};
	    $ev_idx_max = $cnt;
	}
	$cnt++;
    }
    print STDERR "num-ev=$num_ev_max " if ($debug);

    return $events_all[$ev_idx_max];
} # sub get_max_template_events()


sub get_qt_values
{
    my ($clh, $ev_id, $ch, $noise) = @_;

    my $prop_qrs_pos = $clh->get_prop('qrs-pos');
    if (not defined($prop_qrs_pos))
    {
	print STDERR "no 'qrs-pos' event-property in the event-class";
	return undef;
    }
    my $prop_q = $clh->get_prop('ecg-qrs-start');
    if (not defined($prop_q))
    {
	print STDERR "no 'ecg-qrs-start' event-property in the event-class";
	return undef;
    }
    my $prop_t = $clh->get_prop('ecg-t-end');
    if (not defined($prop_t))
    {
	print STDERR "no 'ecg-t-end' event-property in the event-class";
	return undef;
    }

    my $prop_noise = $clh->get_prop('ecg-noise');
    if (not defined($prop_noise))
    {
	print STDERR "no 'ecg-noise' event-property in the event-class";
	return undef;
    }

    # get T-wave amplitude
    my $prop_t_peak1 = $clh->get_prop('ecg-t-peak-1');
    if (not defined($prop_t_peak1))
    {
	print STDERR "no 'ecg-t-peak-1' event-property in the event-class";
	return undef;
    }
    my $prop_t_peak2 = $clh->get_prop('ecg-t-peak-2');

    my $mh = RASCH::_if_meas_handle_from_any_handle($clh->get_handle());
    my $rec = {};
    $rec->{rec} = RASCH::_if_rec_get_first($mh, 0);
    bless $rec, "RArec";

    # store values
    my @res = ();
    my $qrs_pos = $prop_qrs_pos->get_value($ev_id, $ch);
    $res[0] = $qrs_pos + $prop_q->get_value($ev_id, $ch);
    $res[1] = $qrs_pos + $prop_t->get_value($ev_id, $ch);
    $res[2] = 0;  # at this index, the samplerate will be stored
    $res[3] = $prop_noise->get_value($ev_id, $ch);

    my $qrs_st = $rec->get_raw_unit($ch, $res[0], 1)->[0];
    my $t_end = $rec->get_raw_unit($ch, $res[0], 1)->[0];
    my $ref = $t_end;

    my $tp1 = $rec->get_raw_unit($ch, $prop_t_peak1->get_value($ev_id, $ch), 1)->[0];
    if (defined($prop_t_peak2))
    {
	my $tp2 = $rec->get_raw_unit($ch, $prop_t_peak2->get_value($ev_id, $ch), 1)->[0];
	my $a1 = abs($tp2 - $ref);
	my $a2 = abs($tp1 - $ref);
	$res[4] = ($a1 + $a2) / 2;
    }
    else
    {
	$res[4] = abs($tp1 - $ref);
    }

    return \@res;
} # sub get_qt_values()


#################### writing results ####################

sub write_results
{
    my ($template_file, $outfile, $export_noise, $results) = @_;

    my $f = *STDOUT;
    if (defined($outfile))
    {
	open OUT_FILE, ">$outfile" or die "can't create file $outfile: $!\n";
	$f = *OUT_FILE;
    }

    open TEMPL_FILE, "<$template_file" or die "can't open template-file $template_file: $!\n";

    while (<TEMPL_FILE>)
    {
	my $curr_line = $_;
	chomp($curr_line);

	$curr_line =~ m|^patient[\w]+\/([\w]+)$|;
	if (defined($1))
	{
	    my $values = $results->{$1};
	    if ((not defined($values)) || ($values->[0] == 0))
	    {
		print $f $curr_line . "\n";		
	    }
	    else
	    {
		my $scale = 1000 / $values->[2];
		$values->[0] *= $scale;
		$values->[1] *= $scale;
		if ($export_more)
		{
		    print $f $curr_line . "\t$values->[0]\t$values->[1]\t$values->[3]\t$values->[4]\n";
		}
		else
		{
		    print $f $curr_line . "\t$values->[0]\t$values->[1]\n";
		}

		if ($debug)
		{
		    my $qt = $values->[1] - $values->[0];
		    print $curr_line . ": $qt\n";
		}
	    }
	}
	else
	{
	    print $f $curr_line . "\n";
	}
    }
    close(TEMPL_FILE);

    close($f) if (defined($outfile));
} # sub write_results()
