use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

my $lib = '-lrasch';
#$lib = "-llibraschd.lib" if ($^O =~ /win/i);
$lib = "-llibrasch.lib" if ($^O =~ /win/i);

WriteMakefile(
	      'NAME'		=> 'RASCH',
	      'VERSION_FROM'	=> 'RASCH.pm', # finds $VERSION
	      'PREREQ_PM'		=> {}, # e.g., Module::Name => 1.1
	      ($] >= 5.005 ?    ## Add these new keywords supported since 5.005
	       (ABSTRACT => 'object orientated Perl-interface for libRASCH', 
		AUTHOR     => 'Raphael Schneider <librasch@gmail.com>') : ()),
	      'LIBS'		=> ["-L../../ $lib"], # e.g., '-lm'
	      'DEFINE'		=> '', # e.g., '-DHAVE_SOMETHING'
	      # Insert -I. if you add *.h files later:
	      'INC'		=> '-I../../include', # e.g., '-I/usr/include/other'
	      # Un-comment this if you add C files to link with later:
	      # 'OBJECT'		=> '$(O_FILES) ra_perl.o', # link all the C files too
	      );
