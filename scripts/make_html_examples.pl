#! /usr/bin/perl

use strict;

my $base_dir = $ARGV[0] or die "base-directory is missing";
my $name = $ARGV[1] or die "filename is missing\n";

my $template = read_file("$base_dir/template.html");

my @vers = ({ext => 'c', add => 'c', str => 'C-Version'}, {ext => 'pl', add => 'perl', str => 'Perl-Version'},
	    {ext => 'py', add => 'python', str => 'Python-Version'}, {ext => 'm', add => 'm', str => 'Matlab/Octave-Version'});
 
for (@vers)
{
    my $curr = $template;

    my $fn = "$base_dir/$name.$_->{ext}";
    my $source = read_file($fn);
    die "can't open $fn: $!\n" if (not $source);

    $source = interp_html($source);
    $source = "<p><h2>Sourcecode</h2></p>\n<pre>\n" . $source . "</pre>\n";

    $fn = "$base_dir/output/$name.$_->{ext}.out";
    my $out = read_file($fn);
    if ($out)
    {
	$out = interp_html($out);
	$source .= "<p><h2>Output of the example</h2></p>\n<pre>$out</pre>\n";
    }

    $curr =~ s/\#\#title\#\#/$_->{str} of $name/s;
    $curr =~ s/\#\#source\#\#/$source/s;
    
    $fn = "$base_dir/html/$name"."_$_->{add}.html";
    open FILE, ">$fn" or die "can't create file $fn: $!\n";
    print FILE $curr;
    close FILE;

}

exit 0;


sub read_file
{
    my $fn = shift;

    open FILE, "<$fn" or return undef;
    # read complete file in variable $template
    my $f;
    {
	local $/;
	$f = <FILE>;
    }
    close FILE;

    $f =~ s/.*<\!\[CDATA\[\n//s;
    $f =~ s/\n\#\]\]>//s;
    $f =~ s|\n//\]\]>||s;
    $f =~ s|\n\]\]>||s;

    return $f
} # sub read_file()


sub interp_html
{
    my $t = shift;
    
    $t =~ s/&/&amp;/sg; # must be first
    $t =~ s/</&lt;/sg;
    $t =~ s/>/&gt;/sg;
    $t =~ s/\"/&quot;/sg;

    return $t;
} # sub interp_html()
