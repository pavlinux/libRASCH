#! /bin/perl

# cut_man_pages.pl Version 0.1.0 -- save man pages of API functions in seperate files
#
# Author: Raphael Schneider (rasch@med1.med.tum.de)
#
# Changes:
# 27.12.2003: Raphael Schneider (rasch@med1.med.tum.de)
#    - first version
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/cut_man_pages.pl,v 1.1 2003/12/27 22:22:35 rasch Exp $

use strict;

my $file_open = 0;
while (<>)
{
    my $t = $_;
    chomp($t);
    if ($t =~ m/^---function (.+)$/)
    {
	close FILE if ($file_open);
	$file_open = 0;

	open FILE, ">$1" or die "can not create file $1: $!\n";
	$file_open = 1;
	next;
    }
    print FILE $_ if ($file_open);
}
close FILE if ($file_open);

exit 0;
