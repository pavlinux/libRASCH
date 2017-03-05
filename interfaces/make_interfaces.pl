#! /usr/bin/perl

#
# make_interfaces - Version 0.2.5
#
# Copyright (c) 2002-2007 by Raphael Schneider (librasch@gmail.com)
# All rights reserved. This program is free software; you can redistribute
# it and/or modify it under the same terms as Perl itself.
#
#
# Create a XS-file for Perl, a c-file for Python  and header-files and m-files
# for Octave, Matlab and Scilab from the information found in interface_desc.xml.
#
# $Id$
#
# History:
# --------
# ??: first version for Octave and Matlab (rasch@med1.med.tum.de)
# 11.09.2002: added support for Perl (rasch@med1.med.tum.de)
# 13.09.2002: added support for Python (rasch@med1.med.tum.de)
# 16.09.2002: added documentation to Python functions (rasch@med1.med.tum.de)
# 04.03.2003: changes for name-change of library (rasch@med1.med.tum.de)
# 14.08.2003: added support for Scilab (rasch@med1.med.tum.de)
# 01.04.2006: - added 'void' to function definition of 'initRASCH' for Python interface
#             - added check that a variable is defined before using it to avoid
#               warning messages which are annoying (librasch@gmail.com)
# 16.08.2006: added support for Perl and Python specific value type/object
#             (librasch@gmail.com)
# 09.03.2007: changed structure of Matlab prototypes (librasch@gmail.com)
# 16.04.2007: added 'PROTOTYPES: ENABLE' for Perl interface (librasch@gmail.com)
# 15.09.2008: changed 'char *' to 'const char *' in struct definitions to get rid
#             of GCC warnings (librasch@gmail.com)
#


# pragmas
use strict;
use warnings;

# CPAN modules
use XML::LibXML;

# globals
my $gl_version = "0.2.5  15.09.2008";
my %py_arg_types = ( "void *" => "l", "long" => "l", "char *" => "s", "SV *" => "O" );


# input arguments
my $xml_file = "shared/interface_desc.xml";

my $oct_h_file = "octave/ra_octave.h";
my $oct_m_file_folder = "octave/m_files";

my $mat_h_file = "matlab/ra_matlab.h";
my $mat_m_file_folder = "matlab/m_files";

#my $sci_h_file = "scilab/ra_scilab.h";
my $sci_m_file_folder = "scilab/sci_files";

my $perl_xs_file = "perl/RASCH.xs";
my $perl_pod_file = "perl/RASCH.pod";

my $python_file = "python/ra_python.c";


# parse function descriptions
my $parser = XML::LibXML->new();
my $tree = $parser->parse_file($xml_file);
my $root = $tree->getDocumentElement or die "no root-element in xml-file\n";

my $oct_proto = '';
my $oct_func_info = '';

my $mat_proto = '';
my $mat_func_info = '';
my $sci_proto = '';
my $sci_func_info = '';
my $perl_xs = '';
my $perl_pod = '';
my $python = '';
my $python_prototypes = '';
my $python_methods = '';
foreach my $m ($root->findnodes('func'))
  {
    my $name = $m->getAttribute('name');
    my $func_type = $m->getAttribute('type');

    my @para_in = ();
    my @perl_para_in = ();
    my @para_out = ();
    my @perl_para_desc_in = ();
    my $py_arg_list = '';
    foreach my $p ($m->findnodes('para'))
    {
	my $p_name = $p->textContent();
	my $idx = $p->getAttribute('num');
	my $type = $p->getAttribute('type');
	my $perl_type = $p->getAttribute('perl-type');
	my $perl_skip = $p->getAttribute('perl-skip') or 0;

	$para_in[$idx-1] = $p_name if ($type eq "in");
	$perl_para_in[$idx-1] = $p_name if (($type eq "in") && not $perl_skip);
	$para_out[$idx-1] = $p_name if ($type eq "out");
	$perl_para_desc_in[$idx-1] = "  $perl_type p_$p_name" if (($type eq "in") && not $perl_skip); # prefix var-name with 'p_'

	if (($type eq "in") && (defined($py_arg_types{$perl_type})))
	{
	    $py_arg_list .= $py_arg_types{$perl_type};
	}
    }

    my $desc = $m->findvalue('description');

    $desc .= "  Input parameters:\n";
    foreach (@para_in)
    {
	$desc .= "    $_\n",
    }
    $desc .= "\n  Output parameters:\n";
    foreach (@para_out)
    {
	$desc .= "    $_\n",
    }
    $desc .= "\n";

    my @desc_lines = ();
    @desc_lines = split("\n", $desc) if ($desc);
    
    my $outlist = "";
    $outlist = "[" . join(", ", @para_out) . "] =" if (@para_out);
    my $inlist = "";
    $inlist = join(", ", @para_in) if (@para_in);

    my $m_file_add_matlab = $m->findvalue('m-file-add-matlab');
    my $m_file_rep_matlab = $m->findvalue('m-file-replace-matlab');
    my $m_file_add_octave = $m->findvalue('m-file-add-octave');
    my $m_file_rep_octave = $m->findvalue('m-file-replace-octave');
    my $m_file_add_scilab = $m->findvalue('m-file-add-scilab');
    my $m_file_rep_scilab = $m->findvalue('m-file-replace-scilab');

    open(FILE, ">$oct_m_file_folder/$name.m") or die "can't create Octave m-file $name.m: $!\n";
    print FILE "function $outlist $name($inlist)\n";
    foreach (@desc_lines)
    {
	print FILE "% $_\n";
    }
    if ($m_file_rep_octave)
    {
	print FILE "\n$m_file_rep_octave\n";
    }
    else
    {
	print FILE "\n$m_file_add_octave\n" if ($m_file_add_octave);
	if (not $func_type)
	{
	    print FILE "\n\t$outlist ra_octave('$name'";
	    print FILE  ", $inlist" if (@para_in);
	    print FILE ");\n";
	}
    }
    close FILE;

    open(FILE, ">$mat_m_file_folder/$name.m") or die "can't create Matlab m-file $name.m: $!\n";
    print FILE "function $outlist $name($inlist)\n";
    foreach (@desc_lines)
    {
	print FILE "% $_\n";
    }
    if ($m_file_rep_matlab)
    {
	print FILE "\n$m_file_rep_matlab\n";
    }
    else
    {
	print FILE "\n$m_file_add_matlab\n" if ($m_file_add_matlab);
	if (not $func_type)
	{
	    print FILE "\n\t$outlist ra_matlab('$name'";
	    print FILE  ", $inlist" if (@para_in);
	    print FILE ");\n";
	}
    }
    close FILE;

    # for Scilab, only the sci-files are needed; for the c-interface, the Matlab source is taken
    open(FILE, ">$sci_m_file_folder/$name.sci") or die "can't create Scilab function-file $name.sci: $!\n";
    open(FILE_DESC, ">$sci_m_file_folder/$name.cat") or die "can't create Scilab function-file $name.cat: $!\n";
    print FILE "function $outlist $name($inlist)\n";
    foreach (@desc_lines)
    {
	print FILE "// $_\n";
	print FILE_DESC "$_\n";
    }
    close FILE_DESC;
    if ($m_file_rep_scilab)
    {
	print FILE "\n$m_file_rep_scilab\n";
    }
    else
    {
	print FILE "\n$m_file_add_scilab\n" if ($m_file_add_scilab);
	if (not $func_type)
	{
	    print FILE "\n\t$outlist ra_scilab('$name'";
	    print FILE  ", $inlist" if (@para_in);
	    print FILE ");\n";
	}
    }
    close FILE;


    my $n_in = @para_in;
    my $n_out = @para_out;
    my $f = "\t{ \"$name\", $n_in, $n_out, if_$name },\n";

    $oct_proto .= "octave_value_list if_$name(const octave_value_list &args);\n" if (not $func_type);
    $mat_proto .= "void if_$name(mxArray *plhs[], const mxArray *prhs[]);\n" if (not $func_type);

    if (not $func_type)
    {
	$oct_func_info .= $f;
	$mat_func_info .= $f;
    }

    my $xs_code = $m->findvalue('perl-code');
    if ($xs_code)
    {
	my $perl_out = $m->findvalue('perl-out');
	$perl_out = 'void' if (not $perl_out);

	my @t = ();
	foreach (@perl_para_in)
	{
	    push(@t, "p_$_");
	}
	my $perl_inlist = join(", ", @t);
	my $perl_desc = join("\n", @perl_para_desc_in) if (@perl_para_desc_in);
	my $func_name = $name;
	$func_name =~ s/^ra_(.*)/_if_$1/;
	$perl_xs .= "$perl_out\n$func_name(";
	$perl_xs .= $perl_inlist if ($inlist);
	if (@perl_para_desc_in)
	{
	    $perl_xs .= ")\n$perl_desc" ;
	}
	else
	{
	    $perl_xs .= ")";
	}
	$perl_xs .= "$xs_code";
	$perl_xs .= "OUTPUT:\n  RETVAL\n" if ($perl_out ne 'void');

	$perl_xs .= "\n";

	# perl documentation
	my $desc_pod = $m->findvalue('description');
	$perl_pod .= "=item ";
	$perl_pod .= "I<" . join(", ", @para_out) . "> = " if (@para_out );
	$perl_pod .= "B<$name>(";
	$perl_pod .= "I<" . join(", ", @perl_para_in) . ">" if (@perl_para_in);
	$perl_pod .= ");\n\n$desc_pod\n\n";
    }

    my $python_code = $m->findvalue('python-code');
    if ($python_code)
    {
	my $func_name = $name;
	$func_name =~ s/^ra_(.*)/_if_$1/;
	my $py_desc = $m->findvalue('description');
	chomp($py_desc);
	$py_desc =~ s/\"/\\\"/g;
	$py_desc =~ s/\n/\"\n\"/g;
	$python_methods .= "  { \"$func_name\", if_$name, METH_VARARGS, \"$py_desc\"},\n";

	$python .= "static PyObject *\nif_$name(PyObject *self, PyObject *args)\n{\n";
	my $py_init = $m->findvalue('python-code-init');
	$python .= "$py_init\n" if ($py_init);
	$python .= join(";\n", @perl_para_desc_in) . ";\n" if (@perl_para_desc_in);
	$python .= "\n";

	$python =~ s/SV \*/PyObject \*/g;

	my @t = ();
	foreach (@para_in)
	{
	    push(@t, "&p_$_");
	}
	my $para_inlist = join(", ", @t);
	$para_inlist = "NULL" if (not $para_inlist);
	$python .= "  if (!PyArg_ParseTuple(args, \"$py_arg_list\", $para_inlist))\n    return NULL;\n\n";
	
	$python .= $python_code;

	$python .= "} /* if_$name() */\n\n";
    }
  }

# create header-files for octave and matlab
my $oct = init_octave();
$oct .= "\n/******************** prototypes ********************/\n";
$oct .= $oct_proto;
$oct .= "\n/******************** function infos ********************/\n";
$oct .= func_struct_oct();
$oct .= "\nstruct if_ra_func f[] = {\n";
$oct .= $oct_func_info;
$oct .= "};  /* struct if_ra_func f */\n";
$oct .= close_octave();

open(FILE, ">$oct_h_file") or die "Can't create Octave header file $oct_h_file: $!\n";
print FILE $oct;
close FILE;


my $mat = init_matlab();
$mat .= "\n/******************** prototypes ********************/\n";
$mat .= $mat_proto;
$mat .= "\n/******************** function infos ********************/\n";
$mat .= func_struct_mat();
$mat .= "\nstruct if_ra_func f[] = {\n";
$mat .= $mat_func_info;
$mat .= "};  /* struct if_ra_func f */\n";
$mat .= close_matlab();

open(FILE, ">$mat_h_file") or die "Can't create Matlab header file $mat_h_file: $!\n";
print FILE $mat;
close FILE;

# create xs-file for Perl interface
my $xs = init_perl_xs();
$xs .= $perl_xs;
$xs .= close_perl_xs();

open(FILE, ">$perl_xs_file") or die "Can't create Perl XS-file $perl_xs_file: $!\n";
print FILE $xs;
close FILE;

# create pod-file for Perl interface
my $pod = "=head1 NAME\n\n" . pod_name() . "\n\n";
$pod .= "=head1 SYNOPSIS\n\n" . pod_synopsis() . "\n\n";
$pod .= "=head1 DESCRIPTION\n\n" . pod_description() . "\n\n";
$pod .= "=head1 METHODS\n\n=over 4\n\n" . $perl_pod . "=back\n\n";
$pod .= "=head2 EXPORT\n\n" . pod_export() . "\n\n";
$pod .= "=head1 AUTHOR\n\n" . pod_author() . "\n\n";
$pod .= "=head1 SEE ALSO\n\n" . pod_see_also() . "\n\n";
$pod .= "=cut\n";
open(FILE , ">$perl_pod_file") or die "Can't create Perl POD-file $perl_pod_file: $!\n";
print FILE $pod;
close FILE;

# create c-file for Python interface
my $py = init_python();
$py .= $python;
$py .= "\nstatic PyMethodDef RA_methods[] = \{\n$python_methods  \{ NULL, NULL, 0, NULL\}\n\};\n\n";

$py .= close_python();

open(FILE, ">$python_file") or die "Can't create Python extension-file $python_file: $!\n";
print FILE $py;
close FILE;

exit 0;


############################## functions ##############################

sub init_octave
  {
    my $t = "";
    $t .= "#ifndef RA_OCTAVE_H\n";
    $t .= "#define RA_OCTAVE_H\n\n";
    $t .= "/*\n";
    $t .= " * Don't change this file. It was created automatically and every change\n";
    $t .= " * you made will be lost after the next make.\n *\n";
    $t .= " * created using make_interfaces.pl Version $gl_version\n\n";
    $t .= " */\n\n";

    return $t;
  }  # sub init_octave()

sub func_struct_oct
  {
    my $t = '';

    $t .= "struct if_ra_func\n{\n";
    $t .= "\tconst char *func_name;\n";
    $t .= "\tint n_args_in;\n";
    $t .= "\tint n_args_out;\n";
    $t .= "\toctave_value_list (*func)(const octave_value_list &args);\n";
    $t .= "};  /* struct if_ra_func */\n\n";

    return $t;
  }  # sub func_struct_oct()

sub close_octave
  {
    return "\n#endif  /* RA_OCTAVE_H */\n";
  }  # sub close_octave()


sub init_matlab
  {
    my $t = "";
    $t .= "#ifndef RA_MATLAB_H\n";
    $t .= "#define RA_MATLAB_H\n\n";
    $t .= "/*\n";
    $t .= " * Don't change this file. It was created automatically and every change\n";
    $t .= " * you made will be lost after the next make.\n *\n";
    $t .= " * created using make_interfaces.pl Version $gl_version\n\n";
    $t .= " */\n\n";

    return $t;
  }  # sub init_matlab()

sub func_struct_mat
  {
    my $t = '';

    $t .= "struct if_ra_func\n{\n";
    $t .= "\tconst char *func_name;\n";
    $t .= "\tint n_args_in;\n";
    $t .= "\tint n_args_out;\n";
    $t .= "\tvoid (*func)(mxArray *plhs[], const mxArray *prhs[]);\n";
    $t .= "};  /* struct if_ra_func */\n\n";

    return $t;
  }  # sub func_struct_mat()

sub close_matlab
  {
    return "\n#endif  /* RA_MATLAB_H */\n";
  }  # sub close_matlab()


sub init_perl_xs
{
    my $t = '';

    $t .= "/*\n";
    $t .= " * Don't change this file. It was created automatically and every change\n";
    $t .= " * you made will be lost after the next make.\n *\n";
    $t .= " * created using make_interfaces.pl Version $gl_version\n\n";
    $t .= " */\n\n";
    
    $t .= "#include \"EXTERN.h\"\n#include \"perl.h\"\n#include \"XSUB.h\"\n\n";

    $t .= "#include \"ra_xs.h\"\n\n";

    $t .= "MODULE = RASCH  PACKAGE = RASCH\n\n";

    $t .= "PROTOTYPES: ENABLE\n\n";

    return $t
}  # sub init_perl_xs()

sub close_perl_xs
{
    return '';
}  # close_perl_xs()


sub pod_name
{
    return "RASCH - Perl extension for using libRASCH";
}

sub pod_synopsis
{
    return "use RASCH;";
}

sub pod_description
{
    return "Here comes the description of RASCH for Perl. See libRASCH Manual.";
}

sub pod_export
{
    return "None by default.";
}

sub pod_author
{
    return "Raphael Schneider, E<lt>rasch\@med1.med.tum.deE<gt>";
}

sub pod_see_also
{
    return '';
}

sub init_python
{
    my $t = '';

    $t .= "/*\n";
    $t .= " * Don't change this file. It was created automatically and every change\n";
    $t .= " * you made will be lost after the next make.\n *\n";
    $t .= " * created using make_interfaces.pl Version $gl_version\n\n";
    $t .= " */\n\n";
    
    $t .= "#include <Python.h>\n#include <ra.h>\n#include \"ra_python.h\"\n\n";

    return $t;
}  # sub init_python()


sub close_python
{
    my $t = '';
    
    $t .= "void\ninitRASCH_if(void)\n{\n\tPyObject *m, *d, *desc;\n";
    $t .= "\tm = Py_InitModule(\"RASCH_if\", RA_methods);\n";
    $t .= "\td = PyModule_GetDict(m);\n";
    $t .= "\tdesc = PyString_FromString(\"Interface to access libRASCH from Python.\");\n";
    $t .= "\tPyDict_SetItemString(d, \"__doc__\", desc);\n";
    $t .= "}  /* initRASCH_if() */\n";

    return $t;
}  # sub close_python()
