#! /usr/bin/perl

# make_API_reference.pl Version 0.1.2 -- create docbook-file and man-pages for API reference
#
# Copyright 2004 by Raphael Schneider <rasch@med1.med.tum.de>
#
# Some ideas for the documentation tags in the source are taken from doxygen 
# and some ideas and source code was taken from the kernel-doc script (used
# for the documentation of the Linux kernel).
#
# This software falls under the GNU General Public License.
# Please read the COPYING file for more information
#
# Changes:
# 11.02.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - first version
# 01.03.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - fixed problem with comments documenting non-functions
# 27.07.2004: Raphael Schneider (rasch@med1.med.tum.de)
#    - version information is now in ra_version.h
#
# $Header: /home/cvs_repos.bak/librasch/docs/scripts/make_API_reference.pl,v 1.2 2004/07/27 11:11:20 rasch Exp $

use strict;

use File::Spec;

# process input arguments
my $output = 'docbook';
my $internal = 0;
my $librasch_dir = '';
my $structs = 0;

while ($ARGV[0] =~ m/^-(.*)/)
{
    my $cmd = shift @ARGV;
    if ($cmd eq '-docbook')
    {
	$output = 'docbook';
    } elsif ($cmd eq '-man')
    {
	$output = 'man';
    } elsif ($cmd eq '-API')
    {
	$internal = 0;
    } elsif ($cmd eq '-internal')
    {
	$internal = 1;
    } elsif ($cmd =~ m/librasch\=(.+)$/)
    {
	$librasch_dir = $1;
    } elsif ($cmd eq '-structs')
    {
	$structs = 1;
    }
}

my $librasch_version;
if ($librasch_dir ne '')
{
    $librasch_dir = File::Spec->rel2abs($librasch_dir);
    $librasch_version = get_version("$librasch_dir/include");
}

my $man_date = ('January', 'February', 'March', 'April', 'May', 'June', 
		'July', 'August', 'September', 'October', 
		'November', 'December')[(localtime)[4]] . 
  " " . ((localtime)[5]+1900);

# read core-source files
my @all = ();
for (@ARGV)
{
    my $ret = read_source($_);
    push(@all, @$ret);
}

# make core API reference
for (@all)
{
    next if (!$internal && !($_->{name} =~ m/^ra_/));
    if ($output eq 'docbook')
    {
	if ($structs)
	{
	    output_structs_sgml($_);
	}
	else
	{
	    output_function_sgml($_);
	}
    } elsif ($output eq 'man')
    {
	if ($structs)
	{
#	    output_structs_man($_, $man_date, $librasch_version);
	}
	else
	{
	    output_function_man($_, $man_date, $librasch_version);
	}
    }
    else
    {
	die "output format $output is not supported\n";
    }
}

# make core internal API reference (incl. structures)

exit 0;


######################################################################

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


sub read_source
{
    my $file = shift;

    # documentation matches
    my $doc_start = '^/\*\*\s*$'; # Allow whitespace at end of comment start.
    my $doc_end = '\*/';
    my $doc_com = '\s*\*\s*';
    my $empty = '^\s*$';
    my $com = '^/\*\s*$';

    my @data;

    my $state = 0;
    my @doc;
    my $func;

    open(FILE, "<$file") or die "can not open $file: $!\n";
    while (<FILE>)
    {
	next if (m/$empty/);

	# check for start of documentation block
	if (m/$doc_start/) #  && ($state == 0))
	{
	    @doc = ();
	    $state = 1;
	    next;
	}

	# check for end of documentation block
	if (m/$doc_end/ && ($state == 1))
	{
	    $state = 2;
	    $func = '';
	    next;
	}

	# save documentation block in array (remove leading '*')
	if ($state == 1)
	{
	    my $t = $_;
	    $t =~ s/$doc_com//;
	    chomp($t);
	    push(@doc, $t);
	    next;
	}

	# when end of doc-block, look for function definition
	if ($state == 2)
	{
	    chomp($_);
	    $func .= " $_";
	    if (m/\{/)
	    {
		my $entry = make_entry($func, \@doc);
		my $t = $file;
		$t =~ m/\/([^\/]+)$/;
		$entry->{file} = $1;
		push(@data, $entry);
		$state = 0;
	    }
	}
    }
    close (FILE);

    return \@data;
} # sub read_source()


sub make_entry
{
    my $func = shift(@_);
    my @doc = @{shift(@_)};

    # get function definition parts
    $func =~ m/([^\(]+)\(([^\)]*)\)/;
    $func = $1;
    my $args_temp = $2;               # function arguments

    $func =~ s/^\s*//;
    my @t = split(" ", $func);
    my $name = $t[-1];                # function name
    my $l = @t;
    my $ret_type = join(" ", @t[0..$l-2]);  # function return type
    if ($name =~ m/^(\*+)/)
    {
	$ret_type .= " $1";
	$name =~ s/^\*//g;
    }
    # split arguments
    my @args = ();
    if (defined($args_temp) && ($args_temp ne 'void'))
    {
	my @t = split(",", $args_temp);
	for (@t)
	{
	    my @e = split(" ", $_);
	    my $n = $e[-1];
	    my $l = @e;
	    my %a;
	    $a{name} = $n;
	    $a{type} = join(" ", @e[0..$l-2]);
	    if ($a{name} =~ m/^(\*+)/)
	    {
		$a{type} .= " $1";
		$a{name} =~ s/^\*//g;
	    }
	    push(@args, \%a);
	}
    }

    my $brief_desc = '';
    my $desc = '';
    my $empty = '^\s*$';
    for (@doc)
    {
	my $t = $_;
	next if (m/$empty/);

	if ($t =~ m/\\brief\s+(.+)\s*$/)
	{
	    $brief_desc = $1;
	    next;
	}
	
	if (m/\\param\s*<([^>]+)>\s*(.*)\s*$/)
	{
	    my $n = $1;
	    my $d = $2;
	    for (@args)
	    {
		if ($_->{name} eq $n)
		{
		    $_->{desc} = $d;
		    last;
		}
	    }
	    next;
	}

	$desc .= " $_";
    }
    my %ret;
    $ret{name} = $name;
    $ret{ret} = $ret_type;
    $ret{args} = \@args;
    $ret{brief} = $brief_desc;
    $desc =~ s/^\s*//;
    $ret{desc} = $desc;

    return \%ret;
}  # sub make_entry()


# output function in sgml DocBook
sub output_function_sgml
{
    my %args = %{$_[0]};
    my ($parameter, $section);

    my $id = "API-".$args{'name'};
    $id =~ s/[^A-Za-z0-9]/-/g;

    my $head = $args{name};
    if (!($head =~ m/^ra_/))
    {
	$head .= ' (internal)';
    }
    print "<refentry id=\"$id\">\n";
    print "<refmeta>\n";
    print "<refentrytitle><phrase>".$head."</phrase></refentrytitle>\n";
    print "</refmeta>\n";
    print "<refnamediv>\n";
    print " <refname>".$head."</refname>\n";
    print " <refpurpose>\n";
    print "  ";
#    output_highlight ($args{'brief'});
    print "$args{'brief'}";
    print " </refpurpose>\n";
    print "</refnamediv>\n";

    print "<refsynopsisdiv>\n";
    print " <title>Synopsis</title>\n";
    print "  <funcsynopsis><funcprototype>\n";
    print "   <funcdef>".$args{'ret'}." ";
    print "<function>".$args{'name'}." </function></funcdef>\n";

    my $count = 0;
    if ($#{$args{'args'}} >= 0)
    {
	foreach $parameter (@{$args{'args'}})
	{
	    my $type = $parameter->{type};
	    if ($type =~ m/([^\(]*\(\*)\s*\)\s*\(([^\)]*)\)/)
	    {
		# pointer-to-function
		print "   <paramdef>$1<parameter>$parameter->{name}</parameter>)\n";
		print "     <funcparams>$2</funcparams></paramdef>\n";
	    }
	    else
	    {
		print "   <paramdef>".$type;
		print " <parameter>$parameter->{name}</parameter></paramdef>\n";
	    }
	}
    }
    else
    {
	print "  <void>\n";
    }
    print "  </funcprototype></funcsynopsis>\n";
    print "</refsynopsisdiv>\n";

    # print parameters
    print "<refsect1>\n <title>Arguments</title>\n";
    if ($#{$args{'args'}} >= 0)
    {
	print " <variablelist>\n";
	foreach $parameter (@{$args{'args'}})
	{
	    print "  <varlistentry>\n   <term><parameter>$parameter->{name}</parameter></term>\n";
	    print "   <listitem>\n    <para>\n";
#	    $lineprefix="     ";
#	    output_highlight($parameter{desc});
	    print "$parameter->{desc}";
	    print "    </para>\n   </listitem>\n  </varlistentry>\n";
	}
	print " </variablelist>\n";
    }
    else
    {
	print " <para>\n  None\n </para>\n";
    }
    print "</refsect1>\n";

#    output_section_sgml(@_);
#sub output_section_sgml(%) {
#    my %args = %{$_[0]};
#    my $section;    
#    # print out each section
#    $lineprefix="   ";
#    foreach $section (@{$args{'sectionlist'}}) {
#	print "<refsect1>\n <title>$section</title>\n <para>\n";
#	if ($section =~ m/EXAMPLE/i) {
#	    print "<example><para>\n";
#	}
#	output_highlight($args{'sections'}{$section});
#	if ($section =~ m/EXAMPLE/i) {
#	    print "</para></example>\n";
#	}
#	print " </para>\n</refsect1>\n";
#    }
#}

    # print description
    print "<refsect1>\n<title>Description</title>\n<para>\n$args{desc}</para>\n</refsect1>";
    
    # print filename
    print "<refsect1>\n<title>source file</title>\n<para>\n$args{file}</para>\n</refsect1>";

    print "</refentry>\n\n";
} # sub output_function_sgml()


sub output_structs_sgml
{
#    my %args = %{$_[0]};
#    my ($parameter, $section);
#
#    my $id = "API-struct-".$args{'name'};
#    $id =~ s/[^A-Za-z0-9]/-/g;
#
#    my $head = $args{name};
#    if (!($head =~ m/^ra_/))
#    {
#	$head .= ' (internal)';
#    }
#    print "<refentry id=\"$id\">\n";
#    print "<refmeta>\n";
#    print "<refentrytitle><phrase>".$args{'type'}." ".$args{'struct'}."</phrase></refentrytitle>\n";
#    print "</refmeta>\n";
#    print "<refnamediv>\n";
#    print " <refname>".$args{'type'}." ".$args{'struct'}."</refname>\n";
#    print " <refpurpose>\n";
#    print "  ";
##    output_highlight ($args{'purpose'});
#    print "$args{'purpose'}";
#    print " </refpurpose>\n";
#    print "</refnamediv>\n";
#
#    print "<refsynopsisdiv>\n";
#    print " <title>Synopsis</title>\n";
#    print "  <programlisting>\n";
#    print $args{'type'}." ".$args{'struct'}." {\n";
#    foreach $parameter (@{$args{'parameterlist'}}) {
#        ($args{'parameterdescs'}{$parameter} ne $undescribed) || next;
#	$type = $args{'parametertypes'}{$parameter};
#	if ($type =~ m/([^\(]*\(\*)\s*\)\s*\(([^\)]*)\)/) {
#	    # pointer-to-function
#	    print "  $1 $parameter ($2);\n";
#	} elsif ($type =~ m/^(.*?)\s*(:.*)/) {
#	    print "  $1 $parameter$2;\n";
#	} else {
#	    print "  ".$type." ".$parameter.";\n";
#	}
#    }
#    print "};";
#    print "  </programlisting>\n";
#    print "</refsynopsisdiv>\n";
#
#    print " <refsect1>\n";
#    print "  <title>Members</title>\n";
#
#    print "  <variablelist>\n";
#    foreach $parameter (@{$args{'parameterlist'}}) {
#      ($args{'parameterdescs'}{$parameter} ne $undescribed) || next;
#      print "    <varlistentry>";
#      print "      <term>$parameter</term>\n";
#      print "      <listitem><para>\n";
#      output_highlight($args{'parameterdescs'}{$parameter});
#      print "      </para></listitem>\n";
#      print "    </varlistentry>\n";
#    }
#    print "  </variablelist>\n";
#    print " </refsect1>\n";
#
#    # print description
#    print "<refsect1>\n<title>Description</title>\n<para>\n$args{desc}</para>\n</refsect1>";
#    
#    # print filename
#    print "<refsect1>\n<title>source file</title>\n<para>\n$args{file}</para>\n</refsect1>";
#
#    print "</refentry>\n\n";
} # sub output_structs_sgml()


# output function as man page
sub output_function_man
{
    my %args = %{shift(@_)};
    my $man_date = shift;
    my $librasch_version = shift;

    my ($parameter, $section);
    my $count;

    print "---function $args{name}.3\n";
    print ".TH \"$args{name}\" 3 \"$man_date\" \"libRASCH API ($librasch_version)\"\n";

    print ".SH NAME\n";
    print $args{name}." \\- ".$args{brief}."\n";

    print ".SH SYNOPSIS\n";
    print ".B \"".$args{ret}."\" ".$args{name}."\n";
    $count = 0;
    my $parenth = "(";
    my $post = ",";
    foreach my $parameter (@{$args{args}})
    {
	if ($count == $#{$args{args}})
	{
	    $post = ");";
	}
	my $type = $parameter->{type};
	if ($type =~ m/([^\(]*\(\*)\s*\)\s*\(([^\)]*)\)/)
	{
	    # pointer-to-function
	    print ".BI \"".$parenth.$1."\" ".$parameter->{name}." \") (".$2.")".$post."\"\n";
	}
	else
	{
	    $type =~ s/([^\*])$/$1 /;
	    print ".BI \"".$parenth.$parameter->{type}." \" ".$parameter->{name}." \"".$post."\"\n";
	}
	$count++;
	$parenth = "";
    }

    print ".SH ARGUMENTS\n";
    foreach $parameter (@{$args{args}})
    {
	print ".IP \"".$parameter->{name}."\" 12\n";
	print " $parameter->{desc}\n";
    }

    # print description
    print ".SH \"", "DESCRIPTION", "\"\n";
    print "$args{desc}\n";
    
    # print filename
    print ".SH \"", "SOURCE FILE", "\"\n";
    print "$args{file}\n";

    # print Author
    print ".SH AUTHOR\nThis manual page was automatically generated using the documentation in the source file. " .
	"For errors or suggestions please contact Raphael Schneider <rasch\@med1.med.tum.de>\n";
} # sub output_function_man()
