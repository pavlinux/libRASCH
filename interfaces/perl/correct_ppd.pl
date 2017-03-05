use strict;

open FILE, "<$ARGV[0]" or die "can't open file $ARGV[0]: $!\n";
while (<FILE>)
{
    my $line = $_;
    $line =~ s/CODEBASE HREF=\"\"/CODEBASE HREF=\"RASCH.tar.gz\"/;
    print $line;
}
close FILE;

exit 0;
