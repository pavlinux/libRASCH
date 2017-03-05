#! /usr/bin/perl

# set_version.pl Version 0.1.3 -- set version number in build and install scripts
#
# Copyright 2004-2007 by Raphael Schneider <rasch@med1.med.tum.de>
#
# This script is only needed for development on my system.
#
# Changes:
# 11.02.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - first version
# 04.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added 'librasch_confidential_installer.nsi' and 'README'
# 23.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - fixed version-setting for 'README'
# 27.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added version setting in documentation files
# 21.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - version setting for documentations now are done _only_ here
# 16.04.2007: Raphael Schneider (librasch@gmail.com)
#    - added version-setting for 'librasch_installer_binary_dist.nsi'
#
#
# $Id$

use strict;

use File::Spec;

my $librasch_dir = $ARGV[0];

# convert relative path's to absolute path's
$librasch_dir = File::Spec->rel2abs($librasch_dir);

my $version = get_version("$librasch_dir/include");

# set version in Makefile.dist and Makefile
my $cmd = "perl -pi -e \"s/^VERSION =(.*)\$/VERSION = $version/g;\" $librasch_dir/Makefile.dist $librasch_dir/Makefile $librasch_dir/Makefile.incl";
system($cmd);

# set version in librasch-config.dist and librasch-config
$cmd = "perl -pi -e \"s/^version=(.*)\$/version=$version/g;\" $librasch_dir/librasch-config.dist $librasch_dir/librasch-config";
system($cmd);

# set version in librasch-install script for Win32
$cmd = "perl -pi -e \"s/define PRODUCT_VERSION \\\"(.*)\\\"\$/define PRODUCT_VERSION \\\"$version\\\"/g;\" $librasch_dir/librasch_installer.nsi";
system($cmd);
$cmd = "perl -pi -e \"s/define PRODUCT_VERSION \\\"(.*)\\\"\$/define PRODUCT_VERSION \\\"$version\\\"/g;\" $librasch_dir/librasch_installer_binary_dist.nsi";
system($cmd);
$cmd = "perl -pi -e \"s/define PRODUCT_VERSION \\\"(.*)\\\"\$/define PRODUCT_VERSION \\\"$version\\\"/g;\" $librasch_dir/librasch_confidential_installer.nsi";
system($cmd);

# set version in README
$cmd = "perl -pi -e \"s/libRASCH Version(.*)\$/libRASCH Version $version/g;\" $librasch_dir/README";
system($cmd);

# set version in manual.sgml
my $cmd = "perl -pi -e \"s/User Manual for libRASCH-(.*)\</User Manual for libRASCH-$version\</g;\" $librasch_dir/docs/librasch_manual/src/manual.sgml";
system($cmd);

# set version in libRASCH reference Manuel
$cmd = "perl -pi -e \"s/Reference Manual for libRASCH-(.*)\</Reference Manual for libRASCH-$version\</g;\" $librasch_dir/docs/librasch_reference/src/reference.sgml";
system($cmd);

# set version in doxygen control file
$cmd = "perl -pi -e \"s/^PROJECT_NUMBER         =(.*)\$/PROJECT_NUMBER         = $version/g;\" $librasch_dir/docs/source_desc/librasch.doxygen";
system($cmd);

# set version in RASCHlab tutorial
my $raschlab_version = get_raschlab_version("$librasch_dir/raschlab");
$cmd = "perl -pi -e \"s/Tutorial for RASCHlab-(.*)\</Tutorial for RASCHlab-$raschlab_version\</g;\" $librasch_dir/docs/raschlab_tutorial/src/tutorial.sgml";
system($cmd);


exit 0;


############################## subroutines ##############################


sub get_version
{
    my $inc_dir = shift;

    open FILE, "<$inc_dir/ra_version.h" or die "can't open ra_version.h file: $!\n";

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


