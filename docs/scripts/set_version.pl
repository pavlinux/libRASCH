#! /usr/bin/perl

# set_version.pl Version 0.1.1  --  set libRASCH and RASCHlab version numbers in manuals
#
# Copyright 2004 by Raphael Schneider <rasch@med1.med.tum.de>
#
# Changes:
# 11.02.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - first version
#
# 18.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added support for RASCHlab 
#
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/set_version.pl,v 1.1 2004/05/18 14:33:50 rasch Exp $
#

use strict;

use File::Spec;

my $librasch_dir = $ARGV[0];

# convert relative path's to absolute path's
$librasch_dir = File::Spec->rel2abs($librasch_dir);

my $version = get_version("$librasch_dir/include");
my $raschlab_version = get_raschlab_version("$librasch_dir/raschlab");

# set version in manual.sgml
my $cmd = "perl -pi -e \"s/User Manual for libRASCH-(.+)\</User Manual for libRASCH-$version\</g;\" $librasch_dir/docs/manual_src/manual.sgml";
system($cmd);

# set version in doxygen control file
$cmd = "perl -pi -e \"s/^PROJECT_NUMBER         = (.+)\$/PROJECT_NUMBER         = $version/g;\" $librasch_dir/docs/source_desc/librasch.doxygen";
system($cmd);

# set version in RASCHlab tutorial
$cmd = "perl -pi -e \"s/Tutorial for RASCHlab-(.+)\</Tutorial for RASCHlab-$raschlab_version\</g;\" $librasch_dir/docs/raschlab_tutorial/src/tutorial.sgml";
system($cmd);

exit 0;


############################## subroutines ##############################


sub get_version
{
    my $inc_dir = shift;

    open FILE, "<$inc_dir/ra_priv.h" or die "can't open ra_priv.h file: $!\n";

    my $maj;
    my $min;
    my $pl;
    my $extra = '';
    while (<FILE>)
    {
	$maj = $1 if ($_ =~ m/LIBRASCH_MAJ_VERSION[\s]+([^\s]+)/);
	$min = $1 if ($_ =~ m/LIBRASCH_MIN_VERSION[\s]+([^\s]+)/);
	$pl = $1 if ($_ =~ m/LIBRASCH_PATCH_LEVEL[\s]+([^\s]+)/);
	$extra = $1 if ($_ =~ m/LIBRASCH_EXTRA_VERSION[\s]+([^\s]+)/);
	$extra =~ s/\"//g;
    }

    close FILE;

    return undef if ((not defined($maj)) || (not defined($min)) || (not defined($pl)));
	
    return "$maj.$min.$pl$extra";
}  # sub get_version()


sub get_raschlab_version
{
    my $inc_dir = shift;

    open FILE, "<$inc_dir/general/defines.h" or die "can't open defines.h file: $!\n";

    my $version;
    while (<FILE>)
    {
	$version = $1 if ($_ =~ m/RASCHLAB_VERSION[\s]+([^\s]+)/);
	$version =~ s/\"//g;
    }

    close FILE;

    return undef if (not defined($version));
	
    return "$version";
}  # sub get_raschlab_version()


