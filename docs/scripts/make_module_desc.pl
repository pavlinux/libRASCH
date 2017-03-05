#!/usr/bin/perl

#
# make_module_desc - Version 0.2.0
#
# Copyright (c) 2003-2004 by Raphael Schneider (rasch@med1.med.tum.de)
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
# Search all libRASCH plugin-directories and collect from header-files information
# about the plugin (name, description, version, options, results) and build from
# these information docbook-pages for a libRASCH-plugin-reference appendix.
#
# $HEADER: $
#
# History:
# --------
#   03.02.2003: Raphael Schneider (rasch@med1.med.tum.de)
#      - first version
#   10.03.2003: Raphael Schneider (rasch@med1.med.tum.de)
#      - changes needed for new layout of plugin-source
#   21.03.2003: Raphael Schneider (rasch@med1.med.tum.de)
#      - changes needed for name change (libMTK->libRASCH)
#      - added 'description', 'remarks' and 'author' entries
#   11.02.2004: Raphael Schneider (rasch@med1.med.tum.de)
#      - changed names of ra_value_types
#      - version of plugin will be printed
#      - fixed problem in get_struct_line() if a ',' is in the text
#   21.05.2004: Raphael Schneider (rasch@med1.med.tum.de)
#      - defines in plugin header files are now more correctly matched
#      - skip plugin when _somewhere_ in the platform description a 'skip' stand
#      - plugins of one type are now in there own section
#   27.07.2004: Raphael Schneider (rasch@med1.med.tum.de)
#      - skip non-public plugins
#      - remove gettext_noop in 'gettext_noop(string)'
#
# $ Header: $
#

# pragmas
use strict;
#use warnings;

# globals
my %type_map = (RA_VALUE_TYPE_LONG => "long", RA_VALUE_TYPE_DOUBLE => "double", RA_VALUE_TYPE_LONG_ARRAY => "long array",
		RA_VALUE_TYPE_DOUBLE_ARRAY => "double array", RA_VALUE_TYPE_CHAR => "char *", RA_VALUE_TYPE_CHAR_ARRAY => "char **");


# input arguments
# TODO: use options for arguments
my $pl_dir = $ARGV[0] or die "missing plugin-directory (needs to be arg #1)\n";
my $pl_dist_file = $ARGV[1] or die "missing plugin-dist file (needs to be arg #2)\n";
my $out_file = $ARGV[2] or die "missing docbook out-file (needs to be arg #3)\n";

# read list of plugins
open FILE, "<$pl_dir/plugins_list" or die "can't open file with plugin-list: $!\n";
my @plugins = ();
while (<FILE>)
{
    next if ($_ eq "\n");

    my $fn = $_;
    chomp($fn);

    my ($main, $name, $platform) = split("/", $fn);
    next if ($platform =~ m/skip/);
    next if ($platform =~ m/non_public/);

    push(@plugins, "$main/$name");
}
close FILE;

# process all dirs
#
# hash to store infos:
# |- type
# |- name
# |- ver (version)
# |- short_desc
# |- desc
# |- author
# |- options  (array-ref of hash) 
# |  |- name
# |  |- desc
# |  -- type
# |- results  (array-ref of hash)
#    |- name
#    |- desc
#    -- type{
my @desc = ();
#@dirs = ("hrt");
my @pl_access = ();
my @pl_process = ();
my @pl_gui = ();
my @pl_view = ();
my @pl_unknown = ();
for (sort(@plugins))
{
    print "\rprocess dir $_...";

    my $dir = "$pl_dir/$_";
    my $d = process_dir($dir) or next;

    if ($d->{type} eq 'PLUGIN_ACCESS')
    {
	push(@pl_access, $d) ;
    } elsif ($d->{type} eq 'PLUGIN_PROCESS')
    {
	push(@pl_process, $d);
    } elsif ($d->{type} eq 'PLUGIN_GUI')
    {
	push(@pl_gui, $d);
    } elsif ($d->{type} eq 'PLUGIN_VIEW')
    {
	push(@pl_view, $d);
    } else
    {
	push(@pl_unknown, $d);
    }
}

# create docbook pages
print "\rcreate docbook pages...";
my @pages = ();
my $t = "<section id=\"access-plugins\"><title>Access Plugins</title>\n";
push(@pages, $t);
for (@pl_access)
{
    my $entry = make_docbook_entry($_->{name}, $_->{short_desc}, $_->{desc}, $_->{remarks}, $_->{ver}, $_->{author}, $_->{options}, $_->{results});
    push(@pages, $entry);
}
$t = "\n</section>\n";
push(@pages, $t);

$t = "<section id=\"process-plugins\"><title>Process Plugins</title>\n";
push(@pages, $t);
for (@pl_process)
{
    my $entry = make_docbook_entry($_->{name}, $_->{short_desc}, $_->{desc}, $_->{remarks}, $_->{ver}, $_->{author}, $_->{options}, $_->{results});
    push(@pages, $entry);
}
$t = "\n</section>\n";
push(@pages, $t);

$t = "<section id=\"gui-plugins\"><title>GUI/View Plugins</title>\n";
push(@pages, $t);
for (@pl_gui)
{
    my $entry = make_docbook_entry($_->{name}, $_->{short_desc}, $_->{desc}, $_->{remarks}, $_->{ver}, $_->{author}, $_->{options}, $_->{results});
    push(@pages, $entry);
}
for (@pl_view)
{
    my $entry = make_docbook_entry($_->{name}, $_->{short_desc}, $_->{desc}, $_->{remarks}, $_->{ver}, $_->{author}, $_->{options}, $_->{results});
    push(@pages, $entry);
}
$t = "\n</section>\n";
push(@pages, $t);

if (@pl_unknown > 0)
{
    $t = "<section id=\"unknown-plugins\"><title>Plugins with unknown type</title>\n";
    push(@pages, $t);
    for (@pl_unknown)
    {
	my $entry = make_docbook_entry($_->{name}, $_->{short_desc}, $_->{desc}, $_->{remarks}, $_->{ver}, $_->{author}, $_->{options}, $_->{results});
	push(@pages, $entry);
    }
    $t = "\n</section>\n";
    push(@pages, $t);
}

print "\rwrite in file $out_file...";
my $out = join("\n", @pages);
open FILE, ">$out_file" or die "can't create file $out_file: $!\n";
print FILE $out;
close FILE;

print "\rdone\n";

exit 0;


sub process_dir
{
    my $dir = shift;

    my %res = ();
    process_file($_, \%res) for (glob("$dir/*.h"));
    process_file($_, \%res) for (glob("$dir/*.c"));
    process_file($_, \%res) for (glob("$dir/*.cpp"));

    return \%res;
} # sub process_dir()


sub process_file
{
    my $fn = shift;
    my $entry = shift;

    return $entry if ($fn =~ m/resource\.h/);

    # read complete file
    open FILE, "<$fn" or die "can't open file $fn: $!\n";
    my $f;
    {
	local $/;
	$f = <FILE>;
    }
    close FILE;

    # separate comment
    # search for description and author
    my $in_desc = 0;
    my $in_remarks = 0;
    my $text = '';
    for (split("\n", $f))
    {
	if ($in_desc || $in_remarks)
	{
	    if ($_ =~ m/ \*[\s]*$/)
	    {
		$entry->{desc} = $text if ($in_desc);
		$entry->{remarks} = $text if ($in_remarks);
		$in_desc = 0;
		$in_remarks = 0;
		$text = '';
	    }
	    else
	    {
		$_ =~ m/ \*(.*)/;
		$text .= $1;
	    }
	}

	if ($_ =~ m/ \* Description\:[\s]*/)
	{
	    $in_desc = 1;
	    $text = '';
	}
	if ($_ =~ m/ \* Remarks\:[\s]*/)
	{
	    $in_remarks = 1;
	    $text = '';
	}
    }
    $entry->{desc} = $text if ($in_desc);
    $entry->{remarks} = $text if ($in_remarks);

    if ($f =~ m/Author\(s\)\:[\s]*([^\n]+)\n/s)
    {
	$entry->{author} = $1;
    }

    # search for version, name and short description #define's
    if ($f =~ m/ PLUGIN_VERSION[\s]+([^\n]+)\n/s)
    {
	$entry->{ver} = $1;
	$entry->{ver} =~ s/\"//g;
    }
    if ($f =~ m/ PLUGIN_NAME[\s]+([^\n]+)\n/s)
    {
	$entry->{name} = $1;
	$entry->{name} =~ s/gettext_noop\(\"([^\"]+)\"\)/$1/;
	$entry->{name} =~ s/\"//g;
    }
    if ($f =~ m/ PLUGIN_DESC[\s]+([^\n]+)\n/s)
    {
	$entry->{short_desc} = $1;
	$entry->{short_desc} =~ s/gettext_noop\(\"([^\"]+)\"\)/$1/;
	$entry->{short_desc} =~ s/\"//g;
	print "$entry->{name}: $entry->{short_desc}\n";
    }
    if ($f =~ m/ PLUGIN_TYPE[\s]+([^\n]+)\n/s)
    {
	$entry->{type} = $1;
    }

    # search for option-struct
    my $opt = get_struct_lines($f, "struct ra_option_infos");
    $entry->{options} = $opt if (defined $opt);
    
    # search for result-struct
    my $res = get_struct_lines($f, "static struct ra_result_infos");
    $entry->{results} = $res if (defined $res);

    return $entry;
} # sub process_file()


sub get_struct_lines
{
    my $all = shift;
    my $search = shift;

    $all =~ m/$search[^\n]+\n([^;]+)/s;
    my $d = $1;

    my @ret = ();
    my $cnt = 0;
    for ($d =~ /\{([^\}]+)\}/sg)
    {
	m/[^\"]*\"([^\"]*)\"[^\s]*\s*,\s*[^\"]*\"([^\"]*)\"[^\s]*\s*,\s*([A-Za-z_0-9]*)\s*,/;

	my $name = $1;
	my $desc = $2;
	my $type = $3;

	next if (not defined $name);

	my %curr = (name => $name, desc => $desc, type => $type_map{$type});
	push(@ret, \%curr);
	$cnt++;
    }

    return undef if ($cnt <= 0);

    return \@ret;
} # sub get_struct_lines()


#
# make a docbook-reference entry from the signal-data
#
# arguments
#
#  - name of plugin
#  - purpose of plugin
#  - description-text for plugin
#  - reference to the option-hash of the plugin
#  - reference to the result-hash of the plugin
#
sub make_docbook_entry
{
    my $name = shift(@_);
    my $purp = shift(@_);
    my $desc = shift(@_);
    my $remarks = shift(@_);
    my $version = shift(@_);
    my $author = shift(@_);
    my $opt_ref = shift(@_);
    my $res_ref = shift(@_);

    # make meta information
    my $id = "plugin-$name";
    $id =~ s/\//-/g;
    $id =~ s/\+/p/g;    
    my $entry = "<refentry id=\"$id\">\n<refmeta>\n<refentrytitle><phrase>$name</phrase></refentrytitle>\n</refmeta>\n";

    # make name
    $entry .= "<refnamediv>\n<refname>$name</refname>\n<refpurpose>\n$purp\n</refpurpose>\n</refnamediv>\n";

    # make description
    $entry .= "<refsect1>\n<title>Description</title>\n";
    if (defined $desc)
    {
	$entry .= "<para>\n$desc\n</para>\n";
    }
    else
    {
	$entry .= "<para></para>\n";
    }
    $entry .= "</refsect1>\n";
    if (defined $remarks)
    {
	$entry .= "<refsect1>\n<title>Remarks</title>\n<para>\n$remarks</para>\n</refsect1>";
    }

    # make option-table
    if ($opt_ref)
    {
	$entry .= "<refsect1>\n<title>Options</title>\n";

	my $tab = "<table frame=all><title>List of options for plugin $name</title>\n";
	$tab .= "<tgroup cols=3 align=left colsep=1 rowsep=1>\n";
	$tab .= "<thead><row><entry>Name</entry><entry>Description</entry><entry>type</entry></row></thead>\n";
	$tab .= "<tbody>\n";
	for (@$opt_ref)
	{
	    my $e = "<row><entry>$_->{name}</entry>";
	    $e .= "<entry>$_->{desc}</entry>" if (defined($_->{desc}));
	    $e .= "<entry>$_->{type}</entry></row>" if (defined($_->{type}));
	    $tab .= $e . "\n";
	}
	$tab .= "</tbody>\n";
	$tab .= "</tgroup></table>\n";

	$entry .= "$tab</refsect1>\n";
    }

    # make result-table
    if ($res_ref)
    {
	$entry .= "<refsect1>\n<title>Results</title>\n";

	my $tab = "<table frame=all><title>List of results of plugin $name</title>\n";
	$tab .= "<tgroup cols=3 align=left colsep=1 rowsep=1>\n";
	$tab .= "<thead><row><entry>Name</entry><entry>Description</entry><entry>type</entry></row></thead>\n";
	$tab .= "<tbody>\n";
	for (@$res_ref)
	{
	    my $e = "<row><entry>$_->{name}</entry>";
	    $e .= "<entry>$_->{desc}</entry>" if (defined($_->{desc}));
	    $e .= "<entry>$_->{type}</entry></row>" if (defined($_->{type}));
	    $tab .= $e . "\n";
	}
	$tab .= "</tbody>\n";
	$tab .= "</tgroup></table>\n";

	$entry .= "$tab</refsect1>\n";
    }

    if (defined $version)
    {
	$entry .= "<refsect1>\n<title>Version</title>\n<para>\n$version</para>\n</refsect1>";
    }

    # print author
    $entry .= "<refsect1>\n<title>Author(s)</title>\n<para>\n$author</para>\n</refsect1>";

    # "close" entry
    $entry .= "</refentry>";

    return $entry
}  # sub make_docbook_entry
