package RASCH;

use 5.006;
use strict;
use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use RASCH ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
	
);
our $VERSION = '0.8.21';

bootstrap RASCH $VERSION;


=head1

RASCH - Perl-OO interface for libRASCH

=head1 SYNOPSIS

use RASCH;


=head1 DESCRIPTION



=head1 METHODS

This section lists the different objects and their method functions.

=cut

########## RAvalue namespace
{
package RAvalue;

=head1 RAvalue

=cut


=head1

=head2 new

=cut
sub new
{
    my $class = shift;
    my $self = {};

    $self->{value} = undef;
    $self->{value_utf8} = undef;
    $self->{name} = '';
    $self->{desc} = '';
    $self->{info_id} = -1;
    $self->{number} = undef;
    $self->{modifiable} = 0;

    bless $self, $class;

    return $self;
}

sub DESTROY
{
    my $self = shift;

    reset($self);
}

=head1

=head2 is_ok

=cut
sub is_ok
{
    my $self = shift;

    return (defined($self->{value} ? 1 : 0));
}

=head1

=head2 info

=cut
sub info
{
    my $self = shift;
    my $info = (@_ ? shift : undef);

    $self->{info} = $info if (defined($info));

    return $self->{info_id};
}

=head1

=head2 name

=cut
sub name
{
    my $self = shift;
    my $name = (@_ ? shift : undef);

    $self->{name} = $name if (defined($name));

    return $self->{name};
}

=head1

=head2 desc

=cut
sub desc
{
    my $self = shift;
    my $desc = (@_ ? shift : undef);

    $self->{desc} = $desc if (defined($desc));

    return $self->{desc};
}

=head1

=head2 number

=cut
sub number
{
    my $self = shift;
    my $number = (@_ ? shift : undef);

    $self->{number} = $number if (defined($number));

    return $self->{number};
}


=head1

=head2 modifiable

=cut
sub modifiable
{
    my $self = shift;
    my $modifiable = (@_ ? shift : undef);
    
    $self->{modifiable} = $modifiable if (defined($modifiable));

    return $self->{modifiable};
}


=head1

=head2 value

=cut
sub value
{
    my $self = shift;
    my $value = (@_ ? shift : undef);

    
    $self->{value} = $value if (defined($value));

    return $self->{value};
}


=head1

=head2 value_utf8

=cut
sub value_utf8
{
    my $self = shift;
    my $value = (@_ ? shift : undef);

    
    $self->{value_utf8} = $value if (defined($value));

    return $self->{value_utf8};
}


=head1

=head2 reset

=cut
sub reset
{
    my $self = shift;

    if (is_ok($self))
    {
	if (ref($self->{value}) eq 'REF')
	{
	    undef @{$self->{value}};
	}
	if ((ref($self->{value}) eq 'ARRAY') || (ref($self->{value}) eq 'HASH'))
	{
	    undef $self->{value};
	}
    }

    $self->{value} = undef;
}

} ##### end of RAvalue namspace


########## RAfind namespace
{
package RAfind;

=head1 RAfind

=cut


=head1

=head2 new

=cut
sub new
{
    my $class = shift;
    my $self = {};

    $self->{fn} = '';
    $self->{pl} = '';

    bless $self, $class;

    return $self;
}


=head1

=head2 filename

=cut
sub filename
{
    my $self = shift;
    my $fn = (@_ ? shift : undef);

    
    $self->{fn} = $fn if (defined($fn));

    return $self->{fn};
}

=head1

=head2 plugin

=cut
sub plugin
{
    my $self = shift;
    my $pl = (@_ ? shift : undef);

    
    $self->{pl} = $pl if (defined($pl));

    return $self->{pl};
}

} ##### end of RAfind namespace


########## RAattribute namespace
{
package RAattribute;

=head1 RAattribute

=cut


=head1

=head2 new

=cut
sub new
{
    my $class = shift;
    my $handle = (@_ ? shift : undef);
    my $name = (@_ ? shift : '');
    my $value = (@_ ? shift : undef);
    my $self = {};

    $self->{handle} = $handle;
    $self->{name} = $name;
    $self->{value} = $value;

    bless $self, $class;

    return $self;
}


=head1

=head2 value

=cut
sub value
{
    my $self = shift;
    my $val_type = (@_ ? shift : undef);
    my $val = (@_ ? shift : undef);

    if (defined($val))
    {
	_if_eval_attribute_set($self->{handle}, $self->{name}, $val_type, $val);
	$self->{value} = $val;
    }

    return $self->{value};
}


=head1

=head2 name

=cut
sub name
{
    my $self = shift;
    return $self->{name};
}


=head1

=head2 unset

=cut
sub unset
{
    my $self = shift;
    return _if_eval_attribute_unset($self->{handle}, $self->{name});
}

} ##### end of RAattribute namespace


sub get_all_attributes
{
    my $handle = shift;

    if (not defined($handle))
    {
	return undef;
    }

    my ($attrib_list) = _if_eval_attribute_list($handle);
    my @attribs = ();

    for (@{$attrib_list})
    {
	my ($val) = _if_eval_attribute_get($handle, $_);

	my $curr;
	if (not defined($val))
	{
	    $curr = new RAattribute($handle, $_, $val);
	}
	elsif (ref($val) eq 'REF')
	{
	    my @t = ();
	    push(@t, $_) foreach (@{$val});
	    $curr = new RAattribute($handle, $_, @t);

	    undef @{$val};
	}
	elsif (ref($val) eq 'ARRAY')
	{
	    my @t = ();
	    push(@t, $_) foreach ($val);
	    $curr = new RAattribute($handle, $_, @t);

	    undef $val;
	}
	else
	{
	    $curr = new RAattribute($handle, $_, $val);
	}

	push(@attribs, $curr);
    }

    return \@attribs;
} # sub get_all_attributes()




########## RASCH namespace

=head1 libRASCH (RASCH)

blbla

=cut


=head1

=head2 new

$ra = new RASCH

Creates a RASCH object. libRASCH will be initialized and all plugins are loaded
and initialized. When object is destroyed, memory used in libRASCH will be freed and
the plugins will be unloaded (no explicit 'close' or 'destroy' is needed).

=cut
sub new
{
    my $class = shift;
    my $self = {};

    $self->{ra} = _if_lib_init() or die "can't init libRASCH\n";

    bless $self, $class;

    return $self;
}

sub DESTROY
{
    my $self = shift;

    _if_lib_close($self->{ra}) if $self;
}

=head1

=head2 get_handle

$ra_handle = $ra->get_handle()

Returns the ra_handle of the RASCH object. Is needed for the direct communication with
libRASCH not using the OO interface.

=cut
sub get_handle
{
    my $self = shift;

    return $self->{ra};
}

=head1

=head2 get_error

=cut
sub get_error
{
    my $self = shift;

    my ($num, $text) = _if_lib_get_error($self->{ra});

    return ($num, $text);
}


=head1

=head2 use_plugin

=cut
sub use_plugin
{
    my $self = shift;
    my $pl_idx = @_ ? shift : return -1;
    my $use_it = @_ ? shift : return -1;

    _if_lib_use_plugin($self->{ra}, $pl_idx, $use_it);
}


=head1

=head2 get_plugin_by_name

$pl = $ra->get_plugin_by_name($plugin_name, $search_all)

The function will return a RAplugin object for the libRASCH plugin I<$plugin_name>. Because
it is possible to say that plugins should not be used, I<$search_all> controls if the plugin
$plugin_name should be searched in all available plugins (I<$search_all> = 1) or only in
this plugins, which are marked as 'to use' (I<$search_all> = 0).

=cut
sub get_plugin_by_name
{
    my $self = shift;
    my $name = @_ ? shift : return undef;
    my $search_all = @_ ? shift : 0;

    my $h = _if_plugin_get_by_name($self->{ra}, $name, $search_all);
    return undef if ((not defined($h)) || ($h == 0));

    my $pl = {};
    $pl->{plugin} = $h;
    bless $pl, "RAplugin";

    return $pl;
}

=head1

=head2 get_plugin_by_num

$pl = $ra->get_plugin_by_num($plugin_number, $search_all)

The function returns a RAplugin object for the libRASCH plugin number I<$plugin_number>. This
function is needed to get a list of all plugins, when only the number of available plugins
is known.

=cut
sub get_plugin_by_num
{
    my $self = shift;
    my $num = @_ ? shift : return undef;
    my $search_all = @_ ? shift : 0;

    my $h = _if_plugin_get_by_num($self->{ra}, $num, $search_all);
    return undef if ((not defined($h)) || ($h == 0));

    my $pl = {};
    $pl->{plugin} = $h;
    bless $pl, "RAplugin";

    return $pl;
}

=head1

=head2 find_meas

$rafind_arr_ref = $ra->find_meas($directory)

The function returns all supported measurements (as RAfind objects)in the
directory I<$directory>. Each returned measurement can be used to open the measurement with
B<open_meas()>.

=cut
sub find_meas
{
    my $self = shift;
    my $path = @_ ? shift : return undef;

    my ($fn, $pl) = _if_meas_find($self->{ra}, $path);

    my %temp = ();
    my $cnt = 0;
    for (@$fn)
    {
	$temp{$_} = $pl->[$cnt];
	$cnt++;
    }
    
    my @val = ();
    for (sort(keys(%temp)))
    {
	my $curr = new RAfind;
	$curr->filename($_);
	$curr->plugin($temp{$_});
	push(@val, $curr);
    }
    
    return \@val;
}


=head1

=head2 open_meas

$meas = $ra->open_meas($file_name, $open_fast)

The function opens the measurement associated with the file I<$file_name> and returns a
RAmeas object. Because it is sometimes needed to get only the name about a measurement
object, the flag I<$open_fast> can be used. When I<$open_fast> is set to 1, the plugin can during
opening the measurement skip some initialisation, which tooks some time. 

=cut
sub open_meas
{
    my $self = shift;
    my $fn = (@_ ? shift : return undef);
    my $fn_eval = (@_ ? shift : '');
    my $fast = (@_ ? shift : 0);

    my ($t) = _if_meas_open($self->{ra}, $fn, $fn_eval, $fast);
    return undef if ((not defined($t)) || $t == 0);

    my $meas = {};
    $meas->{meas} = $t;

    bless $meas, "RAmeas";

    return $meas;
}

=head1

=head2 new_meas

$meas = $ra->new_meas($directory, $measurement_name)

The function creates the measurement I<$measurement_name> in the directory I<$directory>. The
data format to store the data is 'rasch-file' (libRASCH own file format).

=cut
sub new_meas
{
    my $self = shift;
    my $dir = (@_) ? shift : return undef;
    my $fn = (@_) ? shift : return undef;

    my ($t) = _if_meas_new($self->{ra}, $dir, $fn);
    return undef if ((not defined($t)) || $t == 0);

    my $meas = {};
    $meas->{meas} = $t;

    bless $meas, "RAmeas";

    return $meas;
}

=head1

=head2 get_info

($value, $name, $desc) = $ra->get_info(info => '???')

The function get the information specified by setting the parameter I<info> and returns an array
with the information (I<$value>), the name of the information (I<$name>) and a short description of
the information (I<$desc>). The available informations (and their info-names used to set I<info>) can be
found in the User Manual.

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;

    my ($v, $n, $d, $m, $vu) = _if_lib_info_get($self->{ra}, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}


=head1

=head2 set_info

$ret = $ra->set_info($info_name, $info_type, $info_value)

=cut
sub set_info
{
    my $self = shift;
    my $info = shift;
    my $type = shift;
    my $val = shift;

    my $ret = RASCH::_if_set_info($self->{ra}, $info, $type, $val);

    return $ret;
}


########## RAmeas namespace
{
package RAmeas;

=head1 RAmeas

=cut
  
 
sub DESTROY
{
    my $self = shift;
    if ($self && $self->{meas})
    {
	RASCH::_if_meas_close($self->{meas});
	$self->{meas} = "";
    }
}

=head1

=head2 get_handle

$mh = $meas->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{meas};
}

=head1

=head2 get_info

($value, $name, $desc) = $meas->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;
    
    my ($v, $n, $d, $m, $vu) = RASCH::_if_meas_info_get($self->{meas}, $info);
    
    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}

=head1

=head2 get_info_idx

($value, $name, $desc) = $meas->get_info_idx(index => ???)

=cut
sub get_info_idx
{
    my ($self, %var) = @_;
    my $idx = $var{index} || return undef;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_info_get_by_idx($self->{meas}, 'meas', $idx);
    
    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}


=head1

=head2 set_info

$ret = $meas->set_info($info_name, $info_type, $info_value)

=cut
sub set_info
{
    my $self = shift;
    my $info = shift;
    my $type = shift;
    my $val = shift;

    my $ret = RASCH::_if_set_info($self->{meas}, $info, $type, $val);

    return $ret;
}


=head1

=head2 move

=cut
sub move
{
    my $self = shift;
    my $dir = shift;

    $self->{meas} = RASCH::_if_meas_move($self->{meas}, $dir);
}

=head1

=head2 copy

=cut
sub copy
{
    my $self = shift;
    my $dir = shift;

    return RASCH::_if_meas_copy($self->{meas}, $dir);
}


=head1

=head2 del

=cut
sub del
{
    my $self = shift;

    RASCH::_if_meas_delete($self->{meas});
    $self->{meas} = "";
}


=head1

=head2 save

$meas->save()

=cut
sub save
{
    my $self = shift;
    RASCH::_if_meas_save($self->{meas});
}

=head1

=head2 load_eval

=cut
sub load_eval
{
    my $self = shift;
    my $fn = @_ ? shift : '';

    # TODO: _if_eval_load() function has to be written
#    my $ret = RASCH::_if_eval_load($self->{meas}, $fn);
#    return $ret;
}


=head1

=head2 get_ra_handle

$ra = $meas->get_ra_handle()

=cut
sub get_ra_handle
{
    my $self = shift;
    returm RASCH::_if_ra_lib_handle_from_any_handle($self->{meas});
}    

=head1

=head2 get_first_rec

$rec = $meas->get_first_rec($session_number)

=cut
sub get_first_rec
{
    my $self = shift;
    my $session_num = @_ ? shift : 0;

    my $rec = {};
    $rec->{rec} = RASCH::_if_rec_get_first($self->{meas}, $session_num);
    return undef if ((not defined($rec->{rec})) || $rec->{rec} == 0);

    bless $rec, "RArec";

    return $rec;
}

=head1

=head2 get_default_eval

$eva = $meas->get_default_eval()

=cut
sub get_default_eval
{
    my $self = shift;

    my $eva = {};
    $eva->{eva} = RASCH::_if_eval_get_default($self->{meas});
    return undef if ((not defined($eva->{eva})) || $eva->{eva} == 0);

    bless $eva, "RAeval";

    return $eva;
}

=head1

=head2 get_original_eval

$eva = $meas->get_original_eval()

=cut
sub get_original_eval
{
    my $self = shift;

    my $eva = {};
    $eva->{eva} = RASCH::_if_eval_get_original($self->{meas});
    return undef if ((not defined($eva->{eva})) || $eva->{eva} == 0);

    bless $eva, "RAeval";

    return $eva;
}


=head1

=head2 get_eval_all

$eva_ref = $meas->get_eval_all()

=cut
sub get_eval_all
{
    my $self = shift;

    my $all = RASCH::_if_eval_get_all($self->{meas});
    return undef if (not defined($all));

    my @evals = ();
    for (@$all)
    {
	my $curr = {};
	$curr->{eva} = $_;
	bless $curr, "RAeval";
	push(@evals, $curr);
    }

    return \@evals;
}


=head1

=head2 add_eval

=cut
sub add_eval
{
    my $self = shift;
    my $name = @_ ? shift : 'perl-script';
    my $desc = @_ ? shift : 'added with the Perl interface';
    my $original = @_ ? shift : 0;

    my $eva = {};
    $eva->{eva} = RASCH::_if_eval_add($self->{meas}, $name, $desc, $original);
    return undef if ((not defined($eva->{eva})) || $eva->{eva} == 0);

    bless $eva, "RAeval";

    return $eva;
}


}  ##### end of RAmeas namespace



########## RArec namespace
{
package RArec;

=head1 RArec

=cut


=head1

=head2 get_handle

$rh = $rec->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{rec};
}

=head1

=head2 get_next

$rec_next = $rec->get_next()

=cut
sub get_next
{
    my $self = shift;

    my $rec_next = {};
    $rec_next->{rec} = RASCH::_if_rec_get_next($self->{rec});
    return undef if ((not defined($rec_next->{rec})) || $rec_next->{rec} == 0);

    bless $rec_next, "RArec";

    return $rec_next;
}

=head1

=head2 get_child

$rec_child = $rec->get_child()

=cut
sub get_child
{
    my $self = shift;

    my $rec_child = {};
    $rec_child->{rec} = RASCH::_if_rec_get_first_child($self->{rec});
    return undef if ((not defined($rec_child->{rec})) || $rec_child->{rec} == 0);

    bless $rec_child, "RArec";

    return $rec_child;
}

=head1

=head2 get_info

($value, $name, $desc) = $rec->get_info(info=>'???', dev=>'???', ch=>'???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;
    my $dev = $var{dev};
    my $ch = $var{ch};

    my ($v, $n, $d, $m, $vu);
    if (defined($dev))
    {
	($v, $n, $d, $m, $vu) = RASCH::_if_dev_info_get($self->{rec}, $dev, $info);
    } elsif (defined($ch))
    {
	($v, $n, $d, $m, $vu) = RASCH::_if_ch_info_get($self->{rec}, $ch, $info);
    } else
    {
	($v, $n, $d, $m, $vu) = RASCH::_if_rec_info_get($self->{rec}, $info);
    }

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);
    
    return $val;
}


=head1

=head2 set_info

$ret = $rec->set_info($info_name, $info_type, $info_value)

=cut
sub set_info
{
    my $self = shift;
    my $info = shift;
    my $type = shift;
    my $val = shift;

    my $ret = RASCH::_if_set_info($self->{meas}, $info, $type, $val);

    return $ret;
}


=head1

=head2 get_meas_handle

$mh = $rec->get_meas_handle()

=cut
sub get_meas_handle
{
    my $self = shift;
    return RASCH::_if_meas_handle_from_any_handle($self->{rec});
}

=head1

=head2 get_raw

$data_ref = $rec->get_raw($channel, $start_sample, $number_samples)

=cut
sub get_raw
{
    my $self = shift;
    my $ch = @_ ? shift : 0;
    my $start = @_ ? shift : -1;
    my $num = @_ ? shift : -1;

    my ($data_ref) = RASCH::_if_raw_get($self->{rec}, $ch, $start, $num);

    return $data_ref;
}

=head1

=head2 get_raw_unit

$data_ref = $rec->get_raw_unit($channel, $start_sample, $number_samples)

=cut
sub get_raw_unit
{
    my $self = shift;
    my $ch = @_ ? shift : 0;
    my $start = @_ ? shift : -1;
    my $num = @_ ? shift : -1;

    my ($data_ref) = RASCH::_if_raw_get_unit($self->{rec}, $ch, $start, $num);

    return $data_ref;
}


} ##### end of RArec namespace


########## RAeval namespace
{
package RAeval;

=head1 RAeval

=cut


=head1

=head2 get_handle

$eh = $eva->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{eva};
}


=head1

=head2 get_attributes

$attrib_ref = $eva->get_attributes()

=cut
sub get_attributes
{
    my $self = shift;

    my $attribs = RASCH::get_all_attributes($self->{eva});

    return $attribs;
}


=head1

=head2 get_info

($value, $name, $desc) = $eva->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_eval_info_get($self->{eva}, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}

=head1

=head2 delete

$eh = $eva->delete()

=cut
sub delete
{
    my $self = shift;

    my $ret = RASCH::_if_eval_delete($self->{eva});
    $self->{eva} = undef;

    return $ret;
}


=head1

=head2 get_meas_handle

$mh = $eva->get_meas_handle()

=cut
sub get_meas_handle
{
    my $self = shift;
    return RASCH::_if_meas_handle_from_any_handle($self->{eva});
}

=head1

=head2 set_default

=cut
sub set_default
{
    my $self = shift;
    return RASCH::_if_eval_set_default($self->{eva});
}


=head1

=head2 add_class

$ec = $eva->add_class($id, $name, $desc)

=cut
sub add_class
{
    my $self = shift;
    my $id = (@_ ? shift : undef);
    my $name = (@_ ? shift : '');
    my $desc = (@_ ? shift : '');

    return undef if (not defined($id));

    my $class = {};
    $class->{handle} = RASCH::_if_class_add($self->{eva}, $id, $name, $desc);
    return undef if ((not defined($class->{handle})) || $class->{handle} == 0);

    bless $class, "RAclass";

    return $class;
}


=head1

=head2 add_class_predef

$ec = $eva->add_class_predef($id)

=cut
sub add_class_predef
{
    my $self = shift;
    my $id = (@_ ? shift : undef);

    return undef if (not defined($id));

    my $class = {};
    $class->{handle} = RASCH::_if_class_add_predef($self->{eva}, $id);
    return undef if ((not defined($class->{handle})) || $class->{handle} == 0);

    bless $class, "RAclass";

    return $class;
}


=head1

=head2 get_class

$ec_ref = $eva->get_class($id)

=cut
sub get_class
{
    my $self = shift;
    my $id = (@_ ? shift : '');

    my ($class_hs) = RASCH::_if_class_get($self->{eva}, $id);

    my @all1;
    for (@$class_hs)
    {
	my $curr= {};
	$curr->{handle} = $_;
	bless $curr, "RAclass";
	push(@all1, $curr);
    }

    return \@all1;
}

=head1

=head2 save

=cut
sub save
{
    my $self = shift;
    my $fn = @_ ? shift : '';
    my $save_ascii = @_ ? shift : 0;

    my $ret = RASCH::_if_eval_save(RASCH::_if_meas_handle_from_any_handle($self->{eva}), $fn, $save_ascii);
    return $ret;
}

} ##### end of RAeval namespace


########## RAclass namespace
{
package RAclass;

=head1 RAclass

=cut


=head1

=head2 get_handle

$ech = $ec->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{handle};
}


=head1

=head2 get_attributes

$attrib_ref = $ec->get_attributes()

=cut
sub get_attributes
{
    my $self = shift;

    my $attribs = RASCH::get_all_attributes($self->{handle});

    return $attribs;
}



=head1

=head2 get_info

($value, $name, $desc) = $ec->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_class_info_get($self->{handle}, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}


=head1

=head2 delete

$ok = $ec->delete()

=cut
sub delete
{
    my $self = shift;
    return RASCH::_if_class_delete($self->{handle});
}



=head1

=head2 add_event

$ev_id = $ec->add_event($start, $end)

=cut
sub add_event
{
    my $self = shift;
    my $start = (@_ ? shift : undef);
    my $end = (@_ ? shift : undef);

    return undef if (not defined($start));
    $end = $start if (not defined($end));

    return RASCH::_if_class_add_event($self->{handle}, $start, $end);
}


=head1

=head2 del_event

$ok = $ec->del_event($ev_id)

=cut
sub del_event
{
    my $self = shift;
    my $ev_id = (@_ ? shift : undef);

    return -1 if (not defined($ev_id));

    return RASCH::_if_class_del_event($self->{handle}, $ev_id);
}


=head1

=head2 set_event_pos

$ok = $ec->set_event_pos($ev_id, $start, $end)

=cut
sub set_event_pos
{
    my $self = shift;
    my $ev_id = (@_ ? shift : undef);
    my $start = (@_ ? shift : undef);
    my $end = (@_ ? shift : undef);

    return undef if (not defined($ev_id) || not defined($start));
    $end = $start if (not defined($end));

    return RASCH::_if_class_set_event_pos($self->{handle}, $ev_id, $start, $end);    
}


=head1

=head2 get_event_pos

($start, $end) = $ec->get_event_pos($ev_id)

=cut
sub get_event_pos
{
    my $self = shift;
    my $ev_id = (@_ ? shift : undef);

    return undef if (not defined($ev_id));

    return RASCH::_if_class_get_event_pos($self->{handle}, $ev_id);    
}


=head1

=head2 get_events

$ev_ids = $ec->get_events($start, $end, $complete, $sort)

=cut
sub get_events
{
    my $self = shift;
    my $start = (@_ ? shift : 0);
    my $end = (@_ ? shift : -1);
    my $complete = (@_ ? shift : 0);
    my $sort = (@_ ? shift : 1);

    return RASCH::_if_class_get_events($self->{handle}, $start, $end, $complete, $sort);
}


=head1

=head2 get_prev_event

$ev_id = $ec->get_prev_event($ev_id)

=cut
sub get_prev_event
{
    my $self = shift;
    my $ev_id = (@_ ? shift : undef);

    return undef if (not defined($ev_id));

    return RASCH::_if_class_get_prev_event($self->{handle}, $ev_id);    
}


=head1

=head2 ge_next_event

$ev_id = $ec->get_next_event($ev_id)

=cut
sub get_next_event
{
    my $self = shift;
    my $ev_id = (@_ ? shift : undef);

    return undef if (not defined($ev_id));

    return RASCH::_if_class_get_next_event($self->{handle}, $ev_id);    
}


=head1

=head2 add_prop

$prop = $ec->add_prop($id, $val_type, $num_values, $name, $desc, $unit, $min, $max, $ignore_value)

=cut
sub add_prop
{
    my $self = shift;
    my $id = (@_ ? shift : undef);
    my $val_type = (@_ ? shift : undef);
    my $num_values = (@_ ? shift : 0);
    my $name = (@_ ? shift : '');
    my $desc = (@_ ? shift : '');
    my $unit = (@_ ? shift : '');
    my $min = (@_ ? shift : undef);
    my $max = (@_ ? shift : undef);
    my $ignore_value = (@_ ? shift : undef);

    return undef if (not defined($id) || not defined($val_type));

    my $use_minmax = 0;
    $use_minmax = 1 if (defined($min) && defined($max));

    my $has_ign_value = 0;
    $has_ign_value = 1 if (defined($ignore_value));

    my $prop = {};
    $prop->{handle} = RASCH::_if_prop_add($self->{handle}, $id, $val_type,
					  $num_values, $name, $desc, $unit,
					  $use_minmax, $min, $max,
					  $has_ign_value, $ignore_value);
    return undef if (not defined($prop->{handle}) || ($prop->{handle} == 0));

    bless $prop, "RAprop";
    return $prop;
}


=head1

=head2 add_prop_predef

$prop = $ec->add_prop_predef($id)

=cut
sub add_prop_predef
{
    my $self = shift;
    my $id = (@_ ? shift : undef);

    return undef if (not defined($id));

    my $prop = {};
    $prop->{handle} = RASCH::_if_prop_add_predef($self->{handle}, $id);
    return undef if (not defined($prop->{handle}) || ($prop->{handle} == 0));

    bless $prop, "RAprop";
    return $prop;
}


=head1

=head2 get_prop

$prop = $ec->get_prop($id)

=cut
sub get_prop
{
    my $self = shift;
    my $id = (@_ ? shift : undef);

    return undef if (not defined($id));

    my $prop = {};
    $prop->{handle} = RASCH::_if_prop_get($self->{handle}, $id);
    return undef if ((not defined($prop->{handle})) || ($prop->{handle} == 0));

    bless $prop, "RAprop";
    return $prop;
}


=head1

=head2 get_prop_all

$prop_list = $ec->get_prop_all()

=cut
sub get_prop_all
{
    my $self = shift;

    my $all = RASCH::_if_prop_get_all($self->{handle});
    return undef if (not defined($all));

    my @props = ();
    for (@$all)
    {
	my $prop = {};
	$prop->{handle} = $_;
	bless $prop, "RAprop";
	push(@props, $prop);
    }

    return \@props;
}


=head1

=head2 add_sum

$sum = $ec->add_sum($id, $name, $desc, $dim_names, $dim_units)

=cut
sub add_sum
{
    my $self = shift;
    my $id = (@_ ? shift : undef);
    my $name = (@_ ? shift : '');
    my $desc = (@_ ? shift : '');
    my $dim_names = (@_ ? shift : undef);
    my $dim_units = (@_ ? shift : undef);

    return undef if (not defined($id));

    my $dim1 = @$dim_names;
    my $dim2 = @$dim_units;
    return undef if ($dim1 != $dim2);

    my $sum = {};
    $sum->{handle} = RASCH::_if_sum_add($self->{handle}, $id, $name, $desc, $dim1, $dim_names, $dim_units);
    return undef if (not defined($sum->{handle}) || ($sum->{handle} == 0));

    bless $sum, "RAsum";
    return $sum;
}


=head1

=head2 get_sum

$sum = $ec->get_sum($id)

=cut
sub get_sum
{
    my $self = shift;
    my $id = (@_ ? shift : undef);

    return undef if (not defined($id));

    my ($sum_hs) = RASCH::_if_sum_get($self->{handle}, $id);

    my @all;
    for (@$sum_hs)
    {
	my $curr= {};
	$curr->{handle} = $_;
	bless $curr, "RAsum";
	push(@all, $curr);
    }

    return \@all;
}

} ##### end of RAclass namespace


########## RAprop namespace
{
package RAprop;


=head1 RAprop

=cut


=head1

=head2 get_handle

$eph = $prop->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{handle};
}


=head1

=head2 get_attributes

$attrib_ref = $ec->get_attributes()

=cut
sub get_attributes
{
    my $self = shift;

    my $attribs = RASCH::get_all_attributes($self->{handle});

    return $attribs;
}


=head1

=head2 delete

$ok = $ec->delete()

=cut
sub delete
{
    my $self = shift;
    return RASCH::_if_prop_delete($self->{handle});
}



=head1

=head2 get_info

($value, $name, $desc) = $prop->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_prop_info_get($self->{handle}, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}



=head1

=head2 set_value

$ok = $prop->set_value($ev_id, $ch, $val_type, $value)

=cut
sub set_value
{
    my $self = shift;
    my $ev_id = @_ ? shift : return undef;
    my $val_type = @_ ? shift : return undef;
    my $value = @_ ? shift : return undef;

    return RASCH::_if_prop_set_value($self->{handle}, $ev_id, $val_type, \$value);
}


=head1

=head2 set_value_mass

$ok = $prop->set_value_mass($ev_ids, $chs, $val_type, $values)

=cut
sub set_value_mass
{
    my $self = shift;
    my $ev_ids = @_ ? shift : return undef;
    my $chs = @_ ? shift : return undef;
    my $val_type = @_ ? shift : return undef;
    my $values = @_ ? shift : return undef;

    return RASCH::_if_prop_set_value_mass($self->{handle}, $ev_ids, $chs, $val_type, \$values);
}


=head1

=head2 get_ch

$chs = $prop->get_ch($ev_id)

=cut
sub get_ch
{
    my $self = shift;
    my $ev_id = @_ ? shift : return undef;

    return RASCH::_if_prop_get_ch($self->{handle}, $ev_id);
}


=head1

=head2 get_value

$value = $prop->($ev_id, $ch)

=cut
sub get_value
{
    my $self = shift;
    my $ev_id = @_ ? shift : return undef;
    my $ch = @_ ? shift : return undef;

    return RASCH::_if_prop_get_value($self->{handle}, $ev_id, $ch);
}


=head1

=head2 get_value_all

($ev_ids, $chs, $values) = $prop->get_value_all()

=cut
sub get_value_all
{
    my $self = shift;

    my $val;
    ($val) = RASCH::_if_prop_get_value_all($self->{handle});

    return ($val->[0], $val->[1], $val->[2]);
#    return $val;
}



=head1

=head2 get_events

$ev_ref = $prop->get_events($minmax_type, $min, $max, $ch)

=cut
sub get_events
{
    my $self = shift;
    my $minmax_type = @_ ? shift : '';
    my $min =  @_ ? shift : undef;
    my $max = @_ ? shift : undef;
    my $ch = @_ ? shift : -1;

    my ($ev_ref) = RASCH::_if_prop_get_events($self->{handle}, $minmax_type, $min, $max, $ch);

    return $ev_ref;
}

} ##### end of RAprop namespace


########## RAsum namespace
{
package RAsum;

=head1 RAsum

=cut

=head1

=head2 get_handle

$sh = $sum->get_handle()

=cut
sub get_handle
{
    my $self = shift;
    return $self->{handle};
}


=head1

=head2 get_attributes

$attrib_ref = $sum->get_attributes()

=cut
sub get_attributes
{
    my $self = shift;

    my $attribs = RASCH::get_all_attributes($self->{handle});

    return $attribs;
}


=head1

=head2 delete

$ok = $sum->delete()

=cut
sub delete
{
    my $self = shift;
    return RASCH::_if_sum_delete($self->{handle});
}


=head1

=head2 get_info

($value, $name, $desc) = $prop->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_sum_info_get($self->{handle}, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}


=head1

=head2 add_ch

$ok = $sum->add_ch($ch, $fiducial_offset)

=cut
sub add_ch
{
    my $self = shift;
    my $ch = (@_ ? shift : '');
    my $fid_offset = (@_ ? shift : 0);

    return undef if (not defined($ch));

    return RASCH::_if_sum_add_ch($self->{handle}, $ch, $fid_offset);
}


=head1

=head2 add_part

$part_id = $sum->add_part($ev_ids)

=cut
sub add_part
{
    my $self = shift;
    my $ev_ids = (@_ ? shift : undef);

    return undef if (not defined($ev_ids));

    return RASCH::_if_sum_add_part($self->{handle}, $ev_ids);
}


=head1

=head2 del_part

$ok = $sum->del_part($part_id)

=cut
sub del_part
{
    my $self = shift;
    my $part_id = (@_ ? shift : undef);

    return undef if (not defined($part_id));

    return RASCH::_if_sum_del_part($self->{handle}, $part_id);
}


=head1

=head2 get_part_all

$part_ids = $sum->get_part_all()

=cut
sub get_part_all
{
    my $self = shift;
    return RASCH::_if_sum_get_part_all($self->{handle});
}


=head1

=head2 set_part_data

$ok = $sum->set_part_data($part_id, $ch_idx, $dim, $value_type, $value)

=cut
sub set_part_data
{
    my $self = shift;
    my $part_id = (@_ ? shift : undef);
    my $ch_idx = (@_ ? shift : undef);
    my $dim = (@_ ? shift : undef);
    my $val_type = (@_ ? shift : undef);
    my $value = (@_ ? shift : undef);

    if (not defined($part_id) || not defined($ch_idx) || not defined($dim)
	|| not defined($val_type) || not defined($value))
    {
	return undef;
    }

    return RASCH::_if_sum_set_part_data($self->{handle}, $part_id, $ch_idx,
					$dim, $val_type, $value);
}


=head1

=head2 get_part_data

$data = $sum->get_part_data($part_id, $ch_idx, $dim)

=cut
sub get_part_data
{
    my $self = shift;
    my $part_id = (@_ ? shift : undef);
    my $ch_idx = (@_ ? shift : undef);
    my $dim = (@_ ? shift : undef);

    if (not defined($part_id) || not defined($ch_idx) || not defined($dim))
    {
	return undef;
    }

    return RASCH::_if_sum_get_part_data($self->{handle}, $part_id, $ch_idx, $dim);
}


=head1

=head2 get_part_events

$events = $sum->get_part_events($part_id)

=cut
sub get_part_events
{
    my $self = shift;
    my $part_id = (@_ ? shift : undef);

    return undef if (not defined($part_id));

    return RASCH::_if_sum_get_part_events($self->{handle}, $part_id);
}


=head1

=head2 set_part_events

$sum->set_part_events($part_id, $events)

=cut
sub set_part_events
{
    my $self = shift;
    my $part_id = (@_ ? shift : undef);
    my $events = (@_ ? shift : undef);

    return undef if (not defined($part_id));
    return undef if (not defined($events));

    return RASCH::_if_sum_set_part_events($self->{handle}, $part_id, $events);
}

} ##### end of RAsum namespace


########## RAplugin namespace
{
package RAplugin;

=head1 RAplugin

=cut


=head1

=head2 get_handle

$plh = $pl->get_handle()

=cut
sub get_handle
{
    my ($self) = @_;
    return $self->{plugin};
}

=head1

=head2 get_info

($value, $name, $desc) = $pl->get_info(info => '???')

=cut
sub get_info
{
    my ($self, %var) = @_;
    my $info = $var{info} || return undef;
    my $num = $var{num} || 0;

    my ($v, $n, $d, $m, $vu) = RASCH::_if_plugin_info_get($self->{plugin}, $num, $info);

    my $val = new RAvalue;
    $val->value($v);
    $val->value_utf8($vu) if (defined($vu));
    $val->name($n);
    $val->desc($d);
    $val->modifiable($m);

    return $val;
}

=head1

=head2 use_it

$pl->use_it()

=cut
sub use_it
{
    my $self = shift;
    
    RASCH::_if_lib_use_plugin($self->{plugin}, 1);
}

=head1

=head2 dont_use_it

$pl->dont_use_it()

=cut
sub dont_use_it
{
    my $self = shift;
    
    RASCH::_if_lib_use_plugin($self->{plugin}, 0);
}

=head1

=head2 get_result_info

($name, $desc, $is_default) = $pl->get_result_info($result_number)

=cut
sub get_result_info
{
    my ($self) = shift;
    my $res_num = @_ ? shift : return undef;

    my ($n, $d, $def);
    ($n) = RASCH::_if_result_info_get($self->{plugin}, $res_num, 'res_name');
    ($d) = RASCH::_if_result_info_get($self->{plugin}, $res_num, 'res_desc');
    ($def) = RASCH::_if_result_info_get($self->{plugin}, $res_num, 'res_default');

    return ($n, $d, $def);
}

=head1

=head2 get_process

$proc = $pl->get_process($meas_handle)

=cut
sub get_process
{
    my ($self) = shift;
    my $meas = @_ ? shift : return undef;

    if (ref($meas) ne "RAmeas")
    {
	print STDERR "The function is now expecting a RAmeas class (and no longer the meas-handle).\n";
	return undef;
    }

    # init processing-struct
    my $proc = {};
    $proc->{plugin} = $self->{plugin};
    $proc->{proc} = RASCH::_if_proc_get($meas->get_handle(), $self->{plugin});
    return undef if ((not defined($proc->{proc})) || $proc->{proc} == 0);

    bless $proc, "RAproc";

    return $proc;
}


} ##### end of RAplugin namespace


########## RAproc namespace
{
package RAproc;

#use Devel::Peek;

=head1 RAproc

=cut

sub DESTROY
{
    my $self = shift;

    RASCH::_if_proc_free($self->{proc}) if $self;
}

=head1

=head2 get_handle

$prh = $proc->get_handle()

=cut
sub get_handle
{
    my ($self) = @_;
    return $self->{proc};
}


=head1

=head2 get_result_idx

$proc->get_result_idx($res_ascii_id)

=cut
sub get_result_idx
{
    my $self = shift;
    my $res_ascii_id = shift;

    my $idx = RASCH::_if_proc_get_result_idx($self->{proc}, $res_ascii_id);

    return $idx;
}


=head1

=head2 set_option

$proc->set_option($option_name, $option_type, $option_value)

=cut
sub set_option
{
    my $self = shift;
    my $opt = shift;
    my $type = shift;
    my $val = shift;

    my $ret = RASCH::_if_lib_set_option($self->{proc}, $opt, $type, $val);

    return $ret;
}


=head1

=head2 get_option

=cut
sub get_option
{
    my $self = shift;
    my $opt = shift;

    my $ret = undef;
    ($ret) = RASCH::_if_lib_get_option($self->{proc}, $opt);

    return $ret;
}


=head1

=head2 process

$results = $proc->process()

=cut
sub process
{
    my $self = shift;

    return undef if (RASCH::_if_proc_do($self->{proc}) != 0);
    
   my $ret = undef;
   ($ret) = RASCH::_if_proc_get_result($self->{proc}, $self->{plugin});

    my @val = ();
    for (@$ret)
    {
	my $res = $_;

#	Dump($res);

	my $curr = new RAvalue;
	$curr->name($res->[1]);
#	print "curr: " . $res->[1] . " ref=" . ref($res->[0]) . "\n";
	$curr->desc($res->[2]);
	if (not defined($res->[0]))
	{
	    $curr->value($res->[0]);
	}
	elsif (ref($res->[0]) eq 'REF')
	{
	    my @t = ();
	    push(@t, $_) foreach (@{$res->[0]});
	    $curr->value(@t);

	    undef @{$res->[0]};
	}
	elsif (ref($res->[0]) eq 'ARRAY')
	{
	    my @t = ();
	    push(@t, $_) foreach ($res->[0]);
	    $curr->value(@t);

	    undef $res->[0];
	}
	else
	{
	    $curr->value($res->[0]);
	}

	push(@val, $curr);

	undef @{$res};
    }
    undef @{$ret};
    
    return \@val;
}

} ##### end of RAproc namespace




1;

__END__

=head1 EXPORT

None by default.

=head1 AUTHOR

Raphael Schneider, E<lt>librasch@gmail.comE<gt>

=cut
