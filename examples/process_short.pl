use RASCH; $ra = new RASCH;
print "$_->[1] ($_->[2]) = $_->[0]\n" for (@{$ra->get_plugin_by_name($ARGV[1])->init_process(eva => $ra->open_meas($ARGV[0], 0)->get_def_eval())->process()});
