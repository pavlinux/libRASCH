#! /bin/perl

# build_dist.pl Version 0.4.2 -- build distribution for libRASCH
#
# Copyright 2003-2010 by Raphael Schneider <librasch@gmail.com>
#
# Changes:
# 15.04.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - write Makefiles for Win32
#    - added some allowed file-extensions and files
# 27.04.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - added support for configurable building
# 10.06.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - added examples directory to distribution
# 15.08.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - added support for Scilab files
#    - in devstudio-workspace-file remove not distributed projects
# 28.11.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - libxml2 config is no longer needed when calling make
#    - add 'Makefile.in' to the allowed files
# 30.11.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - use 'system("cp ....")' instead of File::Copy  to keep the file-date
# 05.12.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - added todo-file to distribution
# 08.01.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added man-pages directory to distribution
# 06.02.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added RASCHlab
#    - added .xpm as allowed extension
#    - added 'blib' and 'build' (Perl and Python temporary build directories)
#      as directories which should be skipped
#    - fixed handling of extensions when filename contains a '.'
# 04.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - fixed handling of RASCHlab project in Visual-C++ project workspace
# 24.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added flag that all files in a directory are copied without the
#      check for allowed file-extensions
#    - added database directory with some short sample recordings
# 21.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added handling of new documentation structure
# 22.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added files for Win32 install file
# 08.06-2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - make a distribution for the non-public plugins
# 17.06.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added support for C++ interface
#    - *.dist files contains only distribution specific things
# 01.07.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - added support for translation files
# 27.07.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - adapted to changes in build-system
# 24.10.2006: Raphael Schneider (librasch@gmail.com)
#    - added qt_help.pro file to distribution (needed to get infos about local Qt installation)
# 08.12.2006: Raphael Schneider (librasch@gmail.com)
#    - added support for plot-lib
# 12.04.2007: Raphael Schneider (librasch@gmail.com)
#    - added support for VS2005 project files and project folders
#    - added new extensions and top-most files
# 25.04.2007: Raphael Schneider (librasch@gmail.com)
#    - the howto for building libRASCH on Win32 has moved into the docs folder
# 24.04.2010: Raphael Schneider (librasch@gmail.com)
#    - fixed building Makefile (distclean option)
#
#
# $Id$


use strict;

use File::Find;
use File::Basename;
use File::Copy;
use File::Spec;

my @ext_allow = qw(.c .h .cpp .cc .dsp .dsw .clw .def .ui .pro .pl .xs .pod .pm .py .PL .ppd .xml .m
		   .html .png .jpg .gif .sgml .w32 .mak .rc .rc2 .sci .sce .cat .3 .xpm .sh
		   .manifest .css .xml .pdf .rtf .man .po .pot .mo .txt .vcproj .sln .cmd);
my @file_allow = qw(Makefile.in Makefile Makefile.gtk Makefile_fftw3 Makefile_sing Makefile_own MANIFEST interface.ok COPYING);
my @dir_reject = qw(CVS .svn Debug Release t output html html.ok blib build .tmp);
my @file_part_reject = qw(kdbgrc);
my @topmost_files = qw(Makefile.in Makefile.cmd.in Makefile.incl.in configure qt_help.pro INSTALL README README.raschlab 
		       COPYING ChangeLog librasch.conf librasch-config.in librasch.dsp librasch.dsw librasch.vcproj 
		       librasch.sln librasch.mak todo COPYING.install librasch_installer.nsi);

my $librasch_dir = $ARGV[0];
my $dist_src = $ARGV[1];


# convert relative path's to absolute path's
$librasch_dir = File::Spec->rel2abs($librasch_dir);
$dist_src = File::Spec->rel2abs($dist_src);

# create distribution directory
my $version = get_version("$librasch_dir/include");
my $dist_dir = "$dist_src/librasch-$version";
my_mkdir($dist_dir);
my $dist_dir_np = "$dist_src/librasch-$version-non-public";
my_mkdir($dist_dir_np);

# copy librasch-source
copy_dir("$librasch_dir/src", "$dist_dir/src");
copy_dir("$librasch_dir/include", "$dist_dir/include");
for (@topmost_files)
{
    system("cp -a $librasch_dir/$_ $dist_dir/$_");
    if ($_ eq 'configure')
    {
	system("chmod 755 $dist_dir/$_");
    }
}


# write Makefile.dist (includes only distribution specific things (e.g. version numbers))
open OUT, ">$dist_dir/Makefile.dist";
print OUT "########## distribution specific settings ##########\n";
print OUT "VERSION = $version\n";
$version =~ m/^([^\.]+)/;
print OUT "MAJ_VERSION = $1\n\n";
close OUT;


# write librasch-config.dist
open OUT, ">$dist_dir/librasch-config.dist";
print OUT "########## distribution specific settings ##########\n";
print OUT "version=$version\n\n";
close OUT;


# write libraschpp-config.dist
open OUT, ">$dist_dir/libraschpp-config.dist";
print OUT "########## distribution specific settings ##########\n";
print OUT "version=$version\n\n";
close OUT;


# copy plugins
# get name of distributed plugins
my_mkdir("$dist_dir/plugins");
my_mkdir("$dist_dir/plugins/bin");
my_mkdir("$dist_dir_np/plugins");
my_mkdir("$dist_dir_np/plugins/bin");
open FILE, "<$librasch_dir/plugins/plugins_list" or die "can't open file with list of plugins: $!\n";
my %plugins = ();  # write Makefiles for plugins stored here
my %plugins_np = ();  # write Makefiles for non-public plugins stored here
my @dist_plugin_list = ();
while (<FILE>)
{
    next if ($_ eq "\n");

    my $fn = $_;
    chomp($fn);

    my ($main, $name, $platform) = split("/", $fn);
    my ($todo, $mod) = split("-", $platform);
    next if ($todo eq 'skip');
    if ($todo eq 'non_public')
    {
	my $name_makefile = "$name;$platform";
	if ($plugins_np{$main})
	{
	    push(@{$plugins_np{$main}}, $name_makefile) if ($platform ne "win32");
	}
	else
	{
	    if ($platform ne "win32")
	    {
		my @t = ($name_makefile);
		$plugins_np{$main} = \@t;
	    }
	}
	
	if (not -d "$dist_dir_np/plugins/$main")
	{
	    my_mkdir("$dist_dir_np/plugins/$main");
	}
	copy_dir("$librasch_dir/plugins/$main/$name", "$dist_dir_np/plugins/$main/$name");

	next;
    }

    push(@dist_plugin_list, $fn);

    my $name_makefile = "$name;$platform";
    if ($plugins{$main})
    {
	push(@{$plugins{$main}}, $name_makefile) if ($platform ne "win32");
    }
    else
    {
	if ($platform ne "win32")
	{
	    my @t = ($name_makefile);
	    $plugins{$main} = \@t;
	}
    }

    if (not -d "$dist_dir/plugins/$main")
    {
	my_mkdir("$dist_dir/plugins/$main");
    }
    copy_dir("$librasch_dir/plugins/$main/$name", "$dist_dir/plugins/$main/$name");
}
close FILE;
# create Makefile for plugins
write_makefile("$dist_dir/plugins", 0, keys %plugins);
for (keys %plugins)  # do NOT sort plugins because 'shared' view plugin has to be done first
{
    my $distclean_is_clean = 0;
    if (($_ eq 'access') || ($_ eq 'process'))
    {
	$distclean_is_clean = 1;
    }
    write_makefile("$dist_dir/plugins/$_", $distclean_is_clean, @{$plugins{$_}});
}
#### TODO: write patch file to add non-public plugins in distribution Makefile's !!!!!!

# write plugin list
open FILE, ">$dist_dir/plugins/plugins_list" or die "can't create $dist_dir/plugins/plugin_list: $!\n";
print FILE join("\n", @dist_plugin_list);
close FILE;

# correct DevStudio 6 workspace file
# (will be removed in one of the next versions)
process_devstudio_6($librasch_dir, $dist_dir, \@dist_plugin_list);

# correct Visual Studio 2005 project folder
process_visualstudio_2005($librasch_dir, $dist_dir, \@dist_plugin_list);

# copy translations
copy_dir("$librasch_dir/po", "$dist_dir/po");

# copy interfaces
copy_dir("$librasch_dir/interfaces", "$dist_dir/interfaces");

# copy plot_lin
copy_dir("$librasch_dir/plot_lib", "$dist_dir/plot_lib");

# copy documentation
my_mkdir("$dist_dir/docs");
copy_dir("$librasch_dir/docs/librasch_manual", "$dist_dir/docs/librasch_manual");
copy_dir("$librasch_dir/docs/librasch_reference", "$dist_dir/docs/librasch_reference");
copy_dir("$librasch_dir/docs/raschlab_tutorial", "$dist_dir/docs/raschlab_tutorial");
copy_dir("$librasch_dir/docs/scripts", "$dist_dir/docs/scripts");
system("cp -a $librasch_dir/docs/Makefile $dist_dir/docs/Makefile");

# copy scripts
copy_dir("$librasch_dir/scripts", "$dist_dir/scripts");

# copy examples
copy_dir("$librasch_dir/examples", "$dist_dir/examples");
copy_dir("$librasch_dir/examples/database", "$dist_dir/examples/database", 1);

# copy RASCHlab
copy_dir("$librasch_dir/raschlab", "$dist_dir/raschlab");

# tar & gzip distribution dir
chdir $dist_src;
system("tar czf librasch-$version.tar.gz librasch-$version");

# tar & gzip distribution dir of non-public plugins
chdir $dist_src;
system("tar czf librasch-$version-non-public.tar.gz librasch-$version-non-public");

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


sub my_mkdir
{
    my $dir = shift(@_);

    return if (-d $dir);

    mkdir $dir or die "can't create dir $dir: $!\n";
} # sub my_mkdir()


my ($src_dir, $dest_dir, $copy_all);
sub do_copy
{
    return if (($_ eq '.') || ($_ eq '..'));

    my $fn = $_;

    my $skip = 0;
    # test directories
    if (-d $fn)
    {
	for (@dir_reject)
	{
	    if ($_ eq $fn)
	    {
		$File::Find::prune = 1;
		return;
	    }
	}
    }
    else
    {
	my ($base, $dir, $ext) = fileparse($File::Find::name, '\..*');
	my $ok = 0;
	$ext =~ m/(\.[^\.]+)$/;
	my $ext_end = $1;
	for (@ext_allow)
	{
	    if ($_ eq $ext_end)
	    {
		$ok = 1;
		last;
	    }
	}
	if (not $ok)
	{
	    for (@file_allow)
	    {
		if ($_ eq $fn)
		{
		    $ok = 1;
		    last;
		}
	    }
	}
	if ($ok)
	{
	    for (@file_part_reject)
	    {
		if ($fn =~ m/$_/)
		{
		    $ok = 0;
		    last;
		}
	    }
	}

	$ok = 1 if ($copy_all);

	return if (not $ok);
    }

    # build dest-path
    my $dest = $File::Find::name;
    $dest =~ s/$src_dir/$dest_dir/;

    if (-d $fn)
    {
	my_mkdir($dest);
    }
    else
    {
#	copy($File::Find::name, $dest) or die "can't copy file $File::Find::name to $dest: $!\n";
	system("cp -a $File::Find::name $dest") == 0 or die "can't copy file $File::Find::name to $dest: $?\n";
    }
} # sub do_copy()


sub copy_dir
{
    my $src = shift;
    my $dest = shift;
    my $c = @_ ? shift : 0;

    my_mkdir($dest);

    $src_dir = $src;
    $dest_dir = $dest;
    $copy_all = $c;

    find(\&do_copy, $src);
}  # sub copy_dir()


sub write_makefile
{
    my ($dir, $distclean_is_clean, @dest) = @_;

    my @all = ();
    my @clean = ();
    my @distclean = ();
    my @all_w32 = ();
    my @clean_w32 = ();
    for (@dest)
    {
	my ($name, $platform) = split(";", $_);

	# make entry for Linux
	if ($platform ne "win32")
	{
	    my $s = "\t\@cd $name && \$(MAKE)";
	    push(@all, $s);
	    $s .= " clean";
	    push(@clean, $s);

	    $s = "\t- \@cd $name && \$(MAKE) distclean";
	    push(@distclean, $s);
	}

	# for Win32, we need to get the name of the Makefile which should be used
	if ($platform ne "linux")
	{
	    my @m = glob("$dir/$name/*.mak");
	    my $make = 'Makefile.w32';
	    if (@m)
	    {
		# there should be only one '.mak' file
		my @t = split("/", @m[0]);
		$make = $t[-1];
	    }
	    # make entry for Win32
	    my $s = "\t\@cd $name && \$(MAKE) -f $make";
	    push(@all_w32, $s);
	    $s .= " clean";
	    push(@clean_w32, $s);
	}
    }

    # Makefile for Linux
    open FILE, ">$dir/Makefile" or die "can't create Makefile in $dir: $!\n";
    print FILE "all:\n" . join("\n", @all) . "\n\n";
    print FILE "clean:\n" . join("\n", @clean) . "\n\n";
    if ($distclean_is_clean)
    {
	print FILE "distclean: clean\n\n";
    }
    else
    {
	print FILE "distclean:\n" . join("\n", @distclean) . "\n\n";
    }
    close FILE;

    # Makefile for Win32
    open FILE, ">$dir/Makefile.w32" or die "can't create Makefile.w32 in $dir: $!\n";
    print FILE "all:\n" . join("\n", @all_w32) . "\n\n";
    print FILE "clean:\n" . join("\n", @clean_w32) . "\n\n";
    close FILE;
} # sub write_makefile()


sub process_devstudio_6
{
    my $in = shift;
    my $out = shift;
    my $pl_list = shift;

    push(@$pl_list, "dont_care/librasch/all");
    push(@$pl_list, "dont_care/matlab/all");
    push(@$pl_list, "dont_care/raschlab/all");

    my $fn = "$in/librasch.dsw";
    open FILE_IN, "<$fn" or die "can't open $fn: $!\n";

    my $skip = 0;
    my $res = '';
    while (<FILE_IN>)
    {
	my $line = $_;
	if ($line =~ m/Project:/)
	{
	    my $found = 0;
	    # check if this project is in the distribution
	    for (@$pl_list)
	    {
		my ($type, $name, $platform) = split("/", $_);
		next if (($platform ne 'all') && ($platform ne 'win32'));
		if ($line =~ m/$name/)
		{
		    $found = 1;
		    last;
		}
	    }
	    if ($found == 0)
	    {
		$skip = 1;
	    }
	    else
	    {
		$skip = 0;
	    }
	}
	$skip = 0 if ($line =~ m/Global:/);

	next if ($skip);

	$res .= $line;
    }
    close FILE_IN;

    my $fn = "$out/librasch.dsw";
    open FILE_OUT, ">$fn" or die "can't open $fn: $!\n";
    print FILE_OUT $res;
    close FILE_OUT;
} # sub process_devstudio_6()


sub process_visualstudio_2005
{
    my $in = shift;
    my $out = shift;
    my $pl_list = shift;

    # add sub-projects which are not plugins and which have to
    # be stay in the project folder
    push(@$pl_list, "dont_care/librasch/all");
    push(@$pl_list, "dont_care/matlab/all");
    push(@$pl_list, "dont_care/raschlab/all");
    push(@$pl_list, "dont_care/raschlab_qt/all");
    push(@$pl_list, "dont_care/ra_plot_lib/all");
    push(@$pl_list, "dont_care/cpp_interface/all");

    my $fn = "$in/librasch.sln";
    open FILE_IN, "<$fn" or die "can't open $fn: $!\n";

    my $res = "";
    my @sub_proj_remove = ();
    my $in_global = 0;
    while (<FILE_IN>)
    {
	my $line = $_;
	my $curr_entry = $line;

	$in_global = 1 if ($line =~ m/^Global/);
	$in_global = 0 if ($line =~ m/^EndGlobal/);

	my $skip = 0;
	if ($line =~ m/^Project/)
	{
	    $line =~ m/Project\(\"\{([^\}]+)\}\"\) = \"([^\"]+)\", \"([^\"]+)\", \"\{([^\}]+)\}\"/;
	    my $proj_id = $1;
	    my $sub_proj_name = $2;
	    my $sub_proj_file = $3;
	    my $sub_proj_id = $4;
	    

	    # now get the complete entry
	    while (1)
	    {
		$line = <FILE_IN>;
		$curr_entry .= $line;
		
		last if ($line =~ m/^EndProject/);
	    }

	    # check if this project is in the distribution
	    my $found = 0;
	    for (@$pl_list)
	    {
		my ($type, $name, $platform) = split("/", $_);
		next if (($platform ne 'all') && ($platform ne 'win32'));
		if ($name eq $sub_proj_name)
		{
		    $found = 1;
		    last;
		}
	    }
	    if ($found == 0)
	    {
		$skip = 1;
		push(@sub_proj_remove, $sub_proj_id);
	    }
	}
	next if ($skip);

	if ($in_global)
	{
	    for (@sub_proj_remove)
	    {
		if ($line =~ m/$_/)
		{
		    $skip = 1;
		    last;
		}
	    }
	}
	next if ($skip);

	$res .= $curr_entry;
    }
    close FILE_IN;

    my $fn = "$out/librasch.sln";
    open FILE_OUT, ">$fn" or die "can't open $fn: $!\n";
    print FILE_OUT $res;
    close FILE_OUT;
} # sub process_visualstudio_2005()
