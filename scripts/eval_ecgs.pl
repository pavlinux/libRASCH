#! /usr/bin/perl

#
# Program to perform ECG evaluations
#
# Copyright 2006 by Raphael Schneider (rasch@librasch.org)
#
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
# Version 0.1: rasch@librasch.org (2006-04-28)
#   first version
#

use strict;
use warnings;
$|++;

use RASCH;
use Getopt::Long;

my $debug = 0;

#################### get options ####################
my $ch = -1;  # all channels
GetOptions("ch=s" => \$ch);

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

# start processing the files/dirs
my $ra = new RASCH or die "can't open libRASCH\n";

my $meas_cnt = 0;
for my $curr (@files_use)
{
    my $meas = $ra->open_meas($curr);
    if (defined($meas))
    {
	$meas_cnt++;
	print STDERR "process measurement $curr (#$meas_cnt)...";
	process_meas($ra, $meas, $ch);
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
	    process_meas($ra, $meas, $ch);
	    print STDERR "\n";
	}
    }
}


exit 0;


sub process_meas
{
    my ($ra, $meas, $ch, $t_wave_algo) = @_;

    do_detect($ra, $meas, $ch, $t_wave_algo);
} # sub process_meas()



#################### beat detection code ####################
sub do_detect
{
    my ($ra, $meas, $ch) = @_;

    my $eva = $meas->add_eval("eval_ecgs.pl", "ECG evaluation");
    if (not defined($eva))
    {
	print STDERR "Error creating evaluation";
	return 0;
    }
    
    do_beat_detect($ra, $meas, $eva, $ch);
    my $all_clh = $eva->get_class('heartbeat');
    my $clh = $all_clh->[0];
    if ((not defined($clh)) || ($clh == 0))
    {
	print STDERR "nok";
	return undef;
    }

    do_fiducial($ra, $meas, $clh);
    do_template($ra, $meas, $clh);
    do_ecg($ra, $meas, $clh);

    $eva->save();

    return $clh;
} # sub do_detect()


sub do_beat_detect
{
    my $ra = shift;
    my $meas = shift;
    my $eva = shift;
    my $ch = shift;

    my $pl = $ra->get_plugin_by_name("detect-ecg", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'detect-ecg' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas);
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    $proc->set_option('save_in_eval', 'l', 1);
    $proc->set_option('do_interpolation', 'l', 1);
    $proc->set_option('eh', 'h', $eva->get_handle());
    
    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    if ($ch == -1)
    {
	for (my $i = 0; $i < $num_ch; $i++)
	{
	    my $ch_type = $rec->get_info(info => 'ch_type', ch => $i)->value();
	    if ($ch_type == 1)  # RA_CH_TYPE_ECG = 1
	    {
		push(@chs, $i);
	    }
	}
    }
    else
    {
	$chs[0] = $ch;
    }
    $proc->set_option('ch', 'pl', \@chs);

    my $results = $proc->process();

    return 0;
} # sub do_beat_detect()


sub do_fiducial
{
    my ($ra, $meas, $clh) = @_;

    my $pl = $ra->get_plugin_by_name("fiducial-point", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'fiducial-point' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas);
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    $proc->set_option('use_class', 'l', 1);
    $proc->set_option('clh', 'h', $clh->get_handle());
    $proc->set_option('save_in_class', 'l', 1);
    $proc->set_option('ch', 'l', 0);
    
    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    for (my $i = 0; $i < $num_ch; $i++)
    {
	my $ch_type = $rec->get_info(info => 'ch_type', ch => $i)->value();
	if ($ch_type == 1)  # RA_CH_TYPE_ECG = 1
	{
	    push(@chs, $i);
	}
    }

    $proc->set_option('ch', 'pl', \@chs);

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
    
    my $proc = $pl->get_process($meas);
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    $proc->set_option('use_class', 'l', 1);
    $proc->set_option('clh', 'h', $clh->get_handle());
    $proc->set_option('save_in_class', 'l', 1);
    $proc->set_option('pos_prop', 'c', 'qrs-pos');
    $proc->set_option('templ_name', 'c', 'qrs-template');
    $proc->set_option('templ_corr', 'c', 'qrs-template-corr');
    $proc->set_option('corr_win_before', 'd', 0.05);
    $proc->set_option('corr_win_after', 'd', 0.15);
    $proc->set_option('accept', 'd', 0.85);
    $proc->set_option('slope_accept_low', 'd', 0.6);
    $proc->set_option('slope_accept_high', 'd', 1.4);
    $proc->set_option('template_win_before', 'd', 0.20);
    $proc->set_option('template_win_after', 'd', 0.20);

    my $rec = $meas->get_first_rec();
    my $num_ch = $rec->get_info(info => 'rec_num_channel')->value();

    my @chs = ();
    for (my $i = 0; $i < $num_ch; $i++)
    {
	my $ch_type = $rec->get_info(info => 'ch_type', ch => $i)->value();
	if ($ch_type == 1)  # RA_CH_TYPE_ECG = 1
	{
	    push(@chs, $i);
	}
    }

    $proc->set_option('ch', 'pl', \@chs);

    my $results = $proc->process();

    return 0;
} # sub do_template()


sub do_ecg
{
    my ($ra, $meas, $clh) = @_;

    my $pl = $ra->get_plugin_by_name("ecg", 0);
    if (not defined($pl))
    {
	print STDERR "Can not find 'ecg' plugin";
	return 0;
    }
    
    my $proc = $pl->get_process($meas);
    if (not defined($proc))
    {
	print STDERR "can't initialize processing";
	return -1;
    }

    $proc->set_option('use_class', 'l', 1);
    $proc->set_option('clh', 'h', $clh->get_handle());
    $proc->set_option('save_in_class', 'l', 1);

    my $results = $proc->process();

    return 0;
} # sub do_ecg()
