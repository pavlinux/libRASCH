#! /usr/bin/perl

#
# ra_calc.pl - call libRASCH process-plugin(s) for a list of measurements (or dirs with meas.) 
#
# Copyright 2005-2007 by Raphael Schneider (rasch@librasch.org)
#
# Version 0.1: <rasch@librasch.org> (09.05.2005)
#   first version
# Version 0.2pre1: <rasch@librasch.org> (26.07.2005)
#   started to add code to set plugin-options
#   (missing: * list options
#             * select correct value type from option-infos available via libRASCH API)
# Version 0.2: rasch@librasch.org (?)
#   all the options stuff done (seems to be complete)
# version 0.2.1: rasch@librasch.org (2007-05-14)
#   changed way how RAproc is initialized in Perl bindings; do the needed
#   changes here
# 
# $Id$
#


use strict;
use warnings;

use Getopt::Long;

use RASCH;

$|++;

my %opt_type_desc = (1 => 'value', 2 => 'value', 3 => 'value', 4 => 'string', 6 => 'value array',
		     7 => 'value array', 8 => 'value array', 9 => 'string array');
my %opt_type_map = (1 => 's', 2 => 'l', 3 => 'd', 4 => 'c', 6 => 'ps', 7 => 'pl',
		    8 => 'pd', 9 => 'pc');

my @plugins = ();
my @options_in = ();
my $file = '';
my $add_plugin_name;
my $show_usage;
GetOptions("plugin=s" => \@plugins,
	   "options=s" => \@options_in,
	   "file=s" => \$file,
	   "add-pl-name" => \$add_plugin_name,
	   "help|?" => \$show_usage);

if (defined($show_usage))
{
    print "ra_calc.pl --plugin=<plugin-list> [--options=<opt>] [--file=<out-file>]\n";
    print "           [--add-pl-name] [--help|?] measurement-file/directory\n\n";

    exit 0;
}

@plugins = split(/,/, join(',', @plugins));

@options_in = split(/,/, join(',', @options_in));
my %options = ();
for (@options_in)
{
    my ($name, $value) = split(/=/, $_);
    $options{$name} = $value;

    # check if the plugin for the option was selected
    my ($pl, $opt_name) = split(/:/, $name);
    if ($opt_name eq 'list')
    {
	list_options($pl);
	exit 0;
    }
    my $found = 0;
    for (@plugins)
    {
	if ($pl eq $_)
	{
	    $found = 1;
	    last;
	}
    }
    die "option-setting: the plugin $pl was not selected\n" if (not $found);
}

# if no arguments left -> no files/dirs given to process
exit 0 if ($#ARGV < 0);

if ($#plugins < 0)
{
    print STDERR "please provide a plugin-name on the command-line\n";
    exit -1;
}

my $ra = new RASCH or die "can't open libRASCH\n";
my @pl = ();
my @hdr = ('rec_name');
my @num_res_elem = ();
for my $curr_pl (@plugins)
{
    my $p = $ra->get_plugin_by_name($curr_pl, 0);
    if ((not defined($p)) || ($p == 0))
    {
	 die "can't find plugin $curr_pl\n";
    }
    push(@pl, $p);

    # get result infos for header
    my $val = $p->get_info(info => 'pl_num_results');
    my $num_res = $val->value();

    my $res_ok = 0;
    for (my $i = 0; $i < $num_res; $i++)
    {
	my $v = $p->get_info(info => 'res_default', num => $i)->value();
	next if (not $v);
	
	$v = $p->get_info(info => 'res_name', num => $i)->value();
	if (defined($add_plugin_name))
	{
	    $v = $curr_pl . ":" . $v;
	}

	push(@hdr, $v);
	$res_ok++;
    }
    push(@num_res_elem, $res_ok);
}


my $cnt = 0;
my $meas_cnt = 0;
for my $curr (@ARGV)
{
    my $meas = $ra->open_meas($curr);
    if (defined($meas))
    {
	$meas_cnt++;
	print STDERR "process measurement $curr (#$meas_cnt)...";
	my $ret = process_meas($ra, \@pl, \@plugins, \%options, \@hdr, \@num_res_elem, $meas, $file, $cnt);
	$cnt++ if ($ret > 0);
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
#		exit -1;
	    }
	    my $ret = process_meas($ra, \@pl, \@plugins, \%options, \@hdr, \@num_res_elem, $meas, $file, $cnt);
	    $cnt++ if ($ret > 0);
	    print STDERR "\n";
	}
    }
}

exit 0;


sub list_options
{
    my $pl_name = shift;

    my $ra = new RASCH or die "can't open libRASCH\n";
    my $pl = $ra->get_plugin_by_name($pl_name, 0) or die "can't find plugin $pl_name\n";

    my $desc = $pl->get_info(info => 'pl_desc')->value();
    my $ver = $pl->get_info(info => 'pl_version')->value();
    my $build = $pl->get_info(info => 'pl_build_ts')->value();

    my ($opt_name, $opt_desc, $opt_type) = get_option_infos($pl);

    print "plugin $pl_name $ver ($build)\n\n$desc\n\n";
    my $i = 0;
    for (@{$opt_name})
    {
	print " * $opt_name->[$i] ($opt_type_desc{$opt_type->[$i]})\n";
	if ($opt_desc->[$i] ne '')
	{
	    my $v = $opt_desc->[$i];
	    $v =~ s/\n/\n    /g;
	    print "    $v\n";
	}
	print "\n";
	$i++;
    }
    print "\n";
} # sub list_options()


sub get_option_infos
{
    my $pl = shift;
    
    my $val = $pl->get_info(info => 'pl_num_opt');
    my $num_opt = $val->value();

    my @opt_name = ();
    my @opt_desc = ();
    my @opt_type = ();
    for (my $i = 0; $i < $num_opt; $i++)
    {
	my $v = $pl->get_info(info => 'pl_opt_name', num => $i)->value();
	push(@opt_name, $v);
	$v = $pl->get_info(info => 'pl_opt_desc', num => $i)->value();
	push(@opt_desc, $v);
	$v = $pl->get_info(info => 'pl_opt_type', num => $i)->value();
	push(@opt_type, $v);
    }

    return (\@opt_name, \@opt_desc, \@opt_type);
} # sub get_option_infos()


sub process_meas
{
    my ($ra, $pl_ref, $pl_names, $opt_ref, $hdr_ref, $num_res_elem_ref, $meas, $file, $hdr_avail) = @_;

    my $eva = $meas->get_default_eval();
    my $rec = $meas->get_first_rec(0);
    my $rec_name = $rec->get_info(info => 'rec_name');

    my @res = ($rec_name->value());
    my $cnt = 0;    
    for my $curr (@$pl_ref)
    {
	my $proc = $curr->get_process($meas);

	if (not defined($proc))
	{
	    print STDERR "can't initialize processing";
	    return -1;
	}
	
	my ($opt_name, $opt_desc, $opt_type) = get_option_infos($curr);

	# set options
	for (keys %{$opt_ref})
	{
	    my $opt_id = $_;

	    my ($pl, $name) = split(/:/, $opt_id);
	    last if ($pl ne $pl_names->[$cnt]);

	    my $idx = 0;
	    my $type = '';
	    for (@{$opt_name})
	    {
		if ($name eq $_)
		{
		    $type = $opt_type_map{$opt_type->[$idx]};
		    last;
		}
		$idx++;
	    }
	    die "\n\n  unknown option '$name' in plugin '$pl'\n\n" if ($type eq '');

	    $proc->set_option($name, $type, $opt_ref->{$opt_id});
	}

	my $results = $proc->process();
	if ($#{@$results} < 0)
	{
	    my $num_res = $num_res_elem_ref->[$cnt];
	    for (my $i = 0; $i < $num_res; $i++)
	    {
		my $v = $curr->get_info(info => 'res_default', num => $i)->value();
		next if (not $v);

		push(@res, '');
	    }
	}
	else
	{
	    my $res_cnt = 0;
	    for (@$results)
	    {
		my $val = defined($_->value()) ? $_->value() : '';
		
		my $v = $curr->get_info(info => 'res_default', num => $res_cnt)->value();
		$res_cnt++;
		next if (not $v);
		
		push(@res, $val);
	    }
	}

	$cnt++;
    }
    
    my $f = *STDOUT;
    if ($file ne '')
    {
	if ($hdr_avail)
	{
	    open OUT_FILE, ">>$file" or die "can't open file $file: $!\n";
	}
	else
	{
	    open OUT_FILE, ">$file" or die "can't create file $file: $!\n";
	}

	$f = *OUT_FILE;
    }

    if (not $hdr_avail)
    {
	print $f join("\t", @{$hdr_ref}) . "\n";
    }

    print $f join("\t", @res) . "\n";

    close($f) if ($file ne '');

    return 1;
} # sub process_meas()
