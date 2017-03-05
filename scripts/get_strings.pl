#!/bin/perl -W

use strict;


my @dirs = qw(src src/eval src/shared include raschlab/Qt raschlab/MFC raschlab/general);

my $librasch_root = $ARGV[0] or die "please name the path of libRASCH root\n";
my $po_dir = $ARGV[1] or die "please name directory for the pot-file\n";


# get plugin-list
open FILE, "<$librasch_root/plugins/plugins_list" or die "can't open file with list of plugins: $!\n";
while (<FILE>)
{
    next if ($_ eq "\n");

    my $fn = $_;
    chomp($fn);

    my ($main, $name, $platform) = split("/", $fn);
    my ($todo, $mod) = split("-", $platform);
    next if ($todo eq 'skip');
    push(@dirs, "plugins/$main/$name");
}
close FILE;


# call xgettext for all directories
my $first = 1;
my $po_filename = "librasch";
foreach (@dirs)
{
    my $d = "$librasch_root/$_";
    print STDERR "process $_...";
    my $join = "";
    $join = " -j " if (not $first);
    $first = 0;

    # get files
    my @f = ();
    push (@f, $_) while (<$d/*.c>);
    push (@f, $_) while (<$d/*.cpp>);
    push (@f, $_) while (<$d/*.h>);
    my $cmd = "xgettext $join --omit-header --keyword=tr -d $po_filename -p $po_dir ". join(" ", @f);
    my $ret = system($cmd);
    print STDERR ($ret == 0 ? "ok\n" : "not ok ($ret)\n");
}

# change extension 'po' to 'pot'
my $po_file = "$po_dir/$po_filename" . ".po";
my $pot_file = "$po_dir/$po_filename" . ".pot";
rename($po_file, $pot_file);

# merge existing po-files with new pot
# msgmerge exisiting(with translations).po new.pot > existing.po
while (<$po_dir/*.po>)
{
    print STDERR "process $_ ...";
    my $cmd = "msgmerge -U $_ $pot_file";
    my $ret = system($cmd);
    print STDERR ($ret == 0 ? "ok\n" : "not ok ($ret)\n");
}
