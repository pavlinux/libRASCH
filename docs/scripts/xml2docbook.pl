#! /usr/bin/perl

#
# xml2docbook - Version 0.10pre1
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/xml2docbook.pl,v 1.1 2003/05/13 13:31:15 rasch Exp $
#
# Copyright (c) 2002 Raphael Schneider (rasch@med1.med.tum.de)
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
# Creates reference sections for the libMTK documentation. At the moment the
# following xml-formats are handled:
#  - signal descriptions (for description see signal_xml.desc)
#  - interface descriptions (for description see interface_xml.desc)
#
# It make a docbook reference section from the xml-files and use the format uses in kernel-doc
# (from Linux-documentation).
# 
#
# commandline arguments:
# ----------------------
#  1. type of reference (1: signals, 2: interface)
#  2. signal-description-file (e.g. signal.xml)
#  3. docbook sgml file (e.g. signals.sgml)
#
#
# History:
# --------
# 19.06.2002: first version (rasch@med1.med.tum.de)
# 04.09.2002: - expand script to handle interface-xml description
#             - made make_docbook_entry() more general /rasch)
# 16.09.2002: works now correct for signal-reference
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
my $type = $ARGV[0] or die "reference type is missing\n";
my $in_file = $ARGV[1] or die "input-xml-file is missing\n";
my $out_file = $ARGV[2] or die "docbook outputfile is missing\n";

# open and parse xml-file
my $parser = XML::LibXML->new();
my $tree = $parser->parse_file($in_file);
my $root = $tree->getDocumentElement or die "no root-element in xml-file";

if ($type == 1)
{
    make_signal_reference($root, $out_file);
}
if ($type == 2)
{
    make_interface_reference($root, $out_file);
}

exit 0;


sub make_signal_reference
{
    my $root = shift(@_);
    my $out_file = shift(@_);

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

	my @para_in = ();
	foreach ($sig->findnodes('para'))
	{
	    my $n = $_->getAttribute('name');
	    push(@para_in, $n);
	}
	
	my $entry = make_docbook_entry("SIG", $name, $purp, $desc, \@para_in, \%para_desc_in, \@para_out, \%para_desc_out);
	
	# write signal-entry to docbook-file
	print FILE "\n$entry\n\n\n";
    }
    
    close(FILE);
}  # sub make_signal_reference()


sub make_interface_reference
{
    my $root = shift(@_);
    my $out_file = shift(@_);

    # create docbook-file
    open(FILE, "> $out_file") or die "can't create file $out_file: $!";
    
    # parameter descriptions are not used for interfaces (at the moment)
    my %para_desc_out = ();
    
    # process every signal
    foreach my $ref ($root->findnodes('func'))
    {
	my $name = $ref->getAttribute('name');
	print "process interface-function $name\n";
	
	my $purp = $ref->findvalue('purpose');
	my $desc = $ref->findvalue('description');

	my @para_in = ();
	my @para_out = ();
	my %para_desc_in = ();
	foreach ($ref->findnodes('para'))
	{
	    my $t = $_->getAttribute('type');
	    my $i = $_->getAttribute('num');
	    my $d = $_->getAttribute('desc');
	    my $n = $_->textContent();

	    $para_in[$i-1] = $n if ($t eq "in");
	    $para_out[$i-1] = $n if ($t eq "out");

	    $para_desc_in{$n} = $d if ($t eq "in");
	}
	
	my $entry = make_docbook_entry("I", $name, $purp, $desc, \@para_in, \%para_desc_in, \@para_out, \%para_desc_out);
	
	# write interface-entry to docbook-file
	print FILE "\n$entry\n\n\n";
    }
    
    close(FILE);
}  # sub make_interface_reference()


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
    my $entry = "<refentry id=\"$id_pre-$name\">\n<refmeta>\n<refentrytitle><phrase>$name</phrase></refentrytitle>\n</refmeta>\n";

    # make name
    $entry .= "<refnamediv>\n<refname>$name</refname>\n<refpurpose>\n$purp\n</refpurpose>\n</refnamediv>\n";

    # make synopsis
    $entry .= "<refsynopsisdiv>\n<title>Synopsis</title>\n";
    $entry .= "<funcsynopsis>\n<funcdef>";
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
	    $entry .= "<paramdef><parameter>$_</parameter></paramdef>";
	}
    }
    $entry .= "</funcsynopsis>\n</refsynopsisdiv>\n";

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
	    my $curr = $para_desc_in{$_};

	    my $p_desc = '';
	    my $p_type = '';
	    if ($curr && $id_pre eq "SIG")
	    {
		$p_desc = $curr->findvalue('desc');
		$p_type = $curr->findvalue('type');
		$entry .= "<varlistentry>\n<term><parameter>$_ - $p_type</parameter></term>\n";
	    }

	    if ($curr && $id_pre eq "I")
	    {
		$p_desc = $curr;
		$entry .= "<varlistentry>\n<term><parameter>$_</parameter></term>\n";
	    }

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
