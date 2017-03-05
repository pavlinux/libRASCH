#! /usr/bin/perl

#
# sig2db - Version 0.1.0
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/sig2db.pl,v 1.2 2004/05/21 21:20:02 rasch Exp $
#
# Copyright (c) 2002-2004 Raphael Schneider (rasch@med1.med.tum.de)
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
# Creates reference sections for the libRASCH documentation. At the moment the
# following xml-formats are handled:
#  - signal descriptions (for description see signal_xml.desc)
# 
#
# commandline arguments:
# ----------------------
#  1. type of reference (0: signals, 1: interface)
#  2. signal-description-file (e.g. signal.xml)
#  3. docbook sgml file (e.g. signals.sgml)
#
#
# History:
# --------
# 19.06.2002: first version (rasch@med1.med.tum.de)
# 04.09.2002: - expand script to handle interface-xml description
#             - made make_docbook_entry() more general
# 21.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#   - added missed <funcprototypes> tag
#   corrected comments in the section above
#
#
# Todo:
# -----
# - try to combine signals and interface xml format
# - better handling of input-arguments
#


# pragmas
use strict;
use warnings;


# CPAN modules
use XML::LibXML;


# get and check commandline arguments
my $in_file = $ARGV[0] or die "signal-xml-file is missing\n";
my $out_file = $ARGV[1] or die "docbook outputfile is missing\n";

# open and parse xml-file
my $parser = XML::LibXML->new();
my $tree = $parser->parse_file($in_file);
my $root = $tree->getDocumentElement or die "no root-element in xml-file";

# get parameter descriptions and build hash-array
my %para_desc_in = ();
foreach my $p ($root->findnodes('para_desc'))
{
    my $n = $p->getAttribute('name');
    $para_desc_in{$n} = $p;
}

# create docbook-file
open(FILE, "> $out_file") or die "can't create file $out_file: $!";

# output parameters not used for signals
my @para_out = ();
my %para_desc_out = ();

# process every signal
foreach my $sig ($root->findnodes('signal'))
{
    my $name = $sig->getAttribute('name');
    print "process signal $name\n";

    my $purp = $sig->findvalue('purpose');
    my $desc = $sig->findvalue('desc');
    my @para_in = $sig->findnodes('para');

    my $entry = make_docbook_entry("SIG", $name, $purp, $desc, \@para_in, \%para_desc_in, \@para_out, \%para_desc_out);

    # write signal-entry to docbook-file
    print FILE "\n$entry\n\n\n";
}

close(FILE);

exit 0;


#
# make a docbook-reference entry from the signal-data
#
# arguments
#
#  - name of the signal
#  - purpose of the signal
#  - description-text for the signal
#  - reference to the parameter-array of the signal
#  - reference to the parameter-description-hash for the whole file
#
sub make_docbook_entry
{
    my $id_pre = shift(@_);
    my $name = shift(@_);
    my $purp = shift(@_);
    my $desc = shift(@_);
    my @para_in = @{shift(@_)};
    my %para_desc_in = %{shift(@_)};
    my @para_out = @{shift(@_)};
    my %para_desc_out = %{shift(@_)};

    # do I have input parameters?
    my $no_para_in = 0;
    if (@para_in == 0)
    {
	$no_para_in = 1;
    }
    # do I have output parameters?
    my $no_para_out = 0;
    if (@para_out == 0)
    {
	$no_para_out = 1;
    }

    # make meta information
    my $entry = "<refentry>\n<refmeta>\n<refentrytitle><phrase id=\"$id_pre-$name\">$name</phrase></refentrytitle>\n</refmeta>\n";

    # make name
    $entry .= "<refnamediv>\n<refname>$name</refname>\n<refpurpose>\n$purp\n</refpurpose>\n</refnamediv>\n";

    # make synopsis
    $entry .= "<refsynopsisdiv>\n<title>Synopsis</title>\n";
    $entry .= "<funcsynopsis><funcprototype>\n<funcdef>";
    if ($no_para_out)
    {
	$entry .= " ";
    }
    else
    {
	if (@para_out == 1)
	{
	    $entry .= $para_out[0];
	}
	else
	{
	    $entry .= "[" . join(', ', @para_out) . "]";
	}
	$entry .= " = ";
    }
    $entry .= "<function>$name </function></funcdef>\n";
    if ($no_para_in)
    {
	$entry .= "<paramdef><parameter>none</parameter></paramdef>\n";
    }
    else
    {
	foreach (@para_in)
	{
	    my $p_name = $_->getAttribute('name');
	    $entry .= "<paramdef><parameter>$p_name</parameter></paramdef>";
	}
    }
    $entry .= "</funcprototype></funcsynopsis>\n</refsynopsisdiv>\n";

    # make arguments
    $entry .= "<refsect1>\n<title>Arguments</title>\n<variablelist>\n";
    if ($no_para_in)
    {
	$entry .= "<varlistentry>\n<term><parameter>-</parameter></term>\n";
	$entry .= "<listitem>\n<para>\nno argument\n</para>\n</listitem>\n</varlistentry>\n";
    }
    else
    {
	foreach (@para_in)
	{
	    my $p_name = $_->getAttribute('name');
	    my $curr = $para_desc_in{$p_name} or die "parameter $p_name was not defined\n";
	    my $p_desc = $curr->findvalue('desc');
	    my $p_type = $curr->findvalue('type');
	    
	    $entry .= "<varlistentry>\n<term><parameter>$p_name - $p_type</parameter></term>\n";
	    $entry .= "<listitem>\n<para>\n$p_desc\n</para>\n</listitem>\n</varlistentry>\n";
	}
    }
    $entry .= "</variablelist>\n</refsect1>\n";

    # make description
    $entry .= "<refsect1>\n<title>Description</title>\n<para>\n$desc\n</para>\n</refsect1>\n";

    # "close" entry
    $entry .= "</refentry>";

    return $entry
}  # sub make_docbook_entry
