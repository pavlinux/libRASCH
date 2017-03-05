#! /usr/bin/perl

#
# infos2docbook - Version 0.10pre1
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/infos2docbook.pl,v 1.1 2003/05/13 13:31:14 rasch Exp $
#
# Copyright (c) 2002 Raphael Schneider (rasch@med1.med.tum.de)
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
#
# It make a docbook reference section from the xml-files and use the format uses in kernel-doc
# (from Linux-documentation).
# 
#
# commandline arguments:
# ----------------------
#  1. info-header-file
#  2. docbook sgml file (e.g. infos.sgml)
#
#
# History:
# --------
# 10.09.2002: first version (rasch@med1.med.tum.de)
# 16.09.2002: split table in chunks belonging to the corresponding
#             function (rasch@med1.med.tum.de)
# 21.03.2003: add #define-name of infos in table
#
# Todo:
# -----
#


# pragmas
use strict;
use warnings;
$|++;


# get and check commandline arguments
my $in_file = $ARGV[0] or die "input-file is missing\n";
my $out_file = $ARGV[1] or die "docbook outputfile is missing\n";

open(FILE, "<$in_file") or die "Can't open input-file $in_file: $!\n";
my @tables = ();
my @titles = ();
my $curr = '';
my $cnt = -1;
while (<FILE>)
{
    if ($_ =~ /^.*---------- (.*) \#(.*)\#.*/)
    {
	$cnt++;
	$titles[$cnt] = "List of info-names for $1  --  $2";
	$curr = '';
    }

    next if ($cnt == -1);

    if ($_ =~ /^.*{.*\"(.*)\", (.*) }.*\/\* (.*) \*\/.*/)
    {
	my $e = "<row><entry>$1</entry><entry>$2</entry><entry>$3</entry></row>\n";
	$curr .= $e;
	$tables[$cnt] = $curr;
    }
}
close(FILE);

open(FILE, ">$out_file") or die "Can't create out-file $out_file: $!\n";

my $num = @tables;
for (my $i = 0; $i < $num; $i++)
{
    my $tab = "<table frame=all><title>$titles[$i]</title>\n";
    $tab .= "<tgroup cols=3 align=left colsep=1 rowsep=1>\n";
    $tab .= "<thead><row><entry>Info Name</entry><entry>Info Constant</entry><entry>Info Description</entry></row></thead>\n";
    $tab .= "<tbody>\n";
    $tab .= $tables[$i];
    $tab .= "</tbody>\n";
    $tab .= "</tgroup></table>\n";

    print FILE $tab;
}
close(FILE);

exit 0;
