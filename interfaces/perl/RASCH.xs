/*
 * Don't change this file. It was created automatically and every change
 * you made will be lost after the next make.
 *
 * created using make_interfaces.pl Version 0.2.5  15.09.2008

 */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ra_xs.h"

MODULE = RASCH  PACKAGE = RASCH

PROTOTYPES: ENABLE

void *
_if_lib_init()
    CODE:
     RETVAL = ra_lib_init();
  OUTPUT:
  RETVAL

void
_if_lib_close(p_ra_handle)
  void * p_ra_handle
    CODE:
      ra_lib_close(p_ra_handle);
  
void
_if_lib_get_error(p_ra_handle)
  void * p_ra_handle
PPCODE:
  long err_num;
  char err_text[200];

  err_num = ra_lib_get_error(p_ra_handle, err_text, 200);

  PUSHs(sv_2mortal(newSViv(err_num)));
  PUSHs(sv_2mortal(newSVpv(err_text, strlen(err_text))));

void *
_if_lib_handle_from_any_handle(p_any_handle)
  void * p_any_handle
CODE:
  RETVAL = ra_lib_handle_from_any_handle(p_any_handle);
OUTPUT:
  RETVAL

void
_if_lib_set_option(p_handle, p_option_name, p_option_type, p_option_value)
  void * p_handle
  char * p_option_name
  char * p_option_type
  SV * p_option_value
PPCODE:
  int ret;
  value_handle vh;

  vh = ra_value_malloc();

  ret = sv_to_value(p_option_value, p_option_type, vh);
  if (ret == 0)
    ret = ra_lib_set_option(p_handle, p_option_name, vh);

  ra_value_free(vh);

  PUSHs(sv_2mortal(newSViv(ret)));


void
_if_lib_get_option(p_handle, p_option_name)
  void * p_handle
  char * p_option_name
PPCODE:
    value_handle vh;
    SV *value;

    vh = ra_value_malloc();
    if (ra_lib_get_option(p_handle, p_option_name, vh) != 0)
    {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
    }

    value = value_to_sv(vh);

    ra_value_free(vh);

    PUSHs(value);


void
_if_lib_info_get(p_ra_handle, p_info_name)
  void * p_ra_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_ra_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_plugin_info_get(p_plugin_handle, p_num, p_info_name)
  void * p_plugin_handle
  long p_num
  char * p_info_name
PPCODE:
  value_handle vh;
  int ret;

  vh = ra_value_malloc();
  ra_value_set_number(vh, p_num);
  if ((ret = ra_info_get_by_name(p_plugin_handle, p_info_name, vh)) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_result_info_get(p_plugin_handle, p_result_num, p_info_name)
  void * p_plugin_handle
  long p_result_num
  char * p_info_name
PPCODE:
  value_handle vh;
 
  vh = ra_value_malloc();
  ra_value_set_number(vh, p_result_num);
  if (ra_info_get_by_name(p_plugin_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_meas_info_get(p_meas_handle, p_info_name)
  void * p_meas_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_meas_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);
    
  ra_value_free(vh);


void
_if_obj_info_get(p_meas_handle, p_info_name)
  void * p_meas_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_meas_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_rec_info_get(p_rec_handle, p_info_name)
  void * p_rec_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_rec_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_dev_info_get(p_rec_handle, p_dev_num, p_info_name)
  void * p_rec_handle
  long p_dev_num
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  ra_value_set_number(vh, p_dev_num);
  if (ra_info_get_by_name(p_rec_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_ch_info_get(p_rec_handle, p_channel, p_info_name)
  void * p_rec_handle
  long p_channel
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  ra_value_set_number(vh, p_channel);
  if (ra_info_get_by_name(p_rec_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_eval_info_get(p_eval_handle, p_info_name)
  void * p_eval_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_eval_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_class_info_get(p_class_handle, p_info_name)
  void * p_class_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_class_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_prop_info_get(p_prop_handle, p_info_name)
  void * p_prop_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_prop_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_summary_info_get(p_sum_handle, p_info_name)
  void * p_sum_handle
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  if (ra_info_get_by_name(p_sum_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_sum_dim_info_get(p_sum_handle, p_dim, p_info_name)
  void * p_sum_handle
  long p_dim
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  ra_value_set_number(vh, p_dim);
  if (ra_info_get_by_name(p_sum_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_sum_ch_info_get(p_sum_handle, p_channel, p_info_name)
  void * p_sum_handle
  long p_channel
  char * p_info_name
PPCODE:
  value_handle vh;

  vh = ra_value_malloc();
  ra_value_set_number(vh, p_channel);
  if (ra_info_get_by_name(p_sum_handle, p_info_name, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_info_get_by_idx(p_any_handle, p_info_type, p_index)
  void * p_any_handle
  char * p_info_type
  long p_index
PPCODE:
  value_handle vh;
  long inf_type;

  inf_type = get_info_type(p_info_type);
  if (inf_type < 0)
  {
    PUSHs(&PL_sv_undef);
    return;  
  }

  vh = ra_value_malloc();

  if (ra_info_get_by_idx(p_any_handle, inf_type, p_index, vh) == 0)
    VALUE_TO_STACK(vh);
  else
    PUSHs(&PL_sv_undef);

  ra_value_free(vh);


void
_if_info_set(p_handle, p_info_name, p_info_type, p_info_value)
  void * p_handle
  char * p_info_name
  char * p_info_type
  SV * p_info_value
PPCODE:
  int ret;
  value_handle vh;

  vh = ra_value_malloc();

  ret = sv_to_value(p_info_value, p_info_type, vh);
  if (ret == 0)
    ret = ra_info_set_by_name(p_handle, p_info_name, vh);

  ra_value_free(vh);

  PUSHs(sv_2mortal(newSViv(ret)));


void *
_if_meas_handle_from_any_handle(p_any_handle)
  void * p_any_handle
CODE:
  RETVAL = ra_meas_handle_from_any_handle(p_any_handle);
OUTPUT:
  RETVAL

void
_if_meas_find(p_ra_handle, p_directory)
  void * p_ra_handle
  char * p_directory
PPCODE:
    char **sigs_found = NULL;
    char **plugins = NULL;
    long n_sigs = 0;
    struct ra_find_struct rfs;
    ra_find_handle mfh;
    long l;
    AV *arr_fn;
    AV *arr_pl;

    mfh = ra_meas_find_first(p_ra_handle, p_directory, &rfs);
    if (mfh)
    {
	do
	{
	    ++n_sigs;
	    sigs_found = (char **)realloc(sigs_found, sizeof(char *) * n_sigs);
	    sigs_found[n_sigs-1] = malloc(sizeof(char) * (strlen(rfs.name)+1));
	    strcpy(sigs_found[n_sigs-1], rfs.name);

	    plugins = (char **)realloc(plugins, sizeof(char *) * n_sigs);
	    plugins[n_sigs-1] = malloc(sizeof(char) * (strlen(rfs.plugin)+1));
	    strcpy(plugins[n_sigs-1], rfs.plugin);
	} while (ra_meas_find_next(mfh, &rfs));
	ra_meas_close_find(mfh);
    }
    
    arr_fn = newAV();
    sv_2mortal((SV *)arr_fn);
    arr_pl = newAV();
    sv_2mortal((SV *)arr_pl);

    for (l = 0; l < n_sigs; l++)
    {
      av_store(arr_fn, l, newSVpv(sigs_found[l], strlen(sigs_found[l])));
      free(sigs_found[l]);

      av_store(arr_pl, l, newSVpv(plugins[l], strlen(plugins[l])));
      free(plugins[l]);
    }

    free(sigs_found);
    free(plugins);

    PUSHs(newRV_noinc((SV *)arr_fn));
    PUSHs(newRV_noinc((SV *)arr_pl));


void *
_if_meas_open(p_ra_handle, p_signal_name, p_eval_file, p_open_fast)
  void * p_ra_handle
  char * p_signal_name
  char * p_eval_file
  long p_open_fast
CODE:
  RETVAL = ra_meas_open(p_ra_handle, p_signal_name, p_eval_file, p_open_fast);
OUTPUT:
  RETVAL

void *
_if_meas_new(p_ra_handle, p_dir_name, p_signal_name)
  void * p_ra_handle
  char * p_dir_name
  char * p_signal_name
CODE:
  RETVAL = ra_meas_new(p_ra_handle, p_dir_name, p_signal_name);
OUTPUT:
  RETVAL

void
_if_meas_save(p_meas_handle)
  void * p_meas_handle
CODE:
  ra_meas_save(p_meas_handle);

void
_if_meas_close(p_meas_handle)
  void * p_meas_handle
CODE:
  ra_meas_close(p_meas_handle);

void *
_if_rec_get_first(p_meas_handle, p_session_number)
  void * p_meas_handle
  long p_session_number
CODE:
  RETVAL = ra_rec_get_first(p_meas_handle, p_session_number);
OUTPUT:
  RETVAL

void *
_if_rec_get_next(p_rec_handle)
  void * p_rec_handle
CODE:
  RETVAL = ra_rec_get_next(p_rec_handle);
OUTPUT:
  RETVAL

void *
_if_rec_get_first_child(p_rec_handle)
  void * p_rec_handle
CODE:
  RETVAL = ra_rec_get_first_child(p_rec_handle);
OUTPUT:
  RETVAL

void
_if_raw_get(p_rec_handle, p_channel, p_start_sample, p_num_samples)
  void * p_rec_handle
  long p_channel
  long p_start_sample
  long p_num_samples
PPCODE:
  AV* arr;
  DWORD *buf_raw = NULL;
  long l, n;

  New(0, buf_raw, p_num_samples, DWORD);
  SAVEFREEPV(buf_raw);
  n = ra_raw_get(p_rec_handle, (unsigned int)p_channel, (size_t)p_start_sample, (size_t)p_num_samples, buf_raw, NULL);

  arr = newAV();
  sv_2mortal((SV *)arr);
  for (l = 0; l < n; l++)
    av_store(arr, l, newSVnv(buf_raw[l]));

  PUSHs(newRV_noinc((SV *)arr));


void
_if_raw_get_unit(p_rec_handle, p_channel, p_start_sample, p_num_samples)
  void * p_rec_handle
  long p_channel
  long p_start_sample
  long p_num_samples
PPCODE:
  AV* arr;
  double *buf= NULL;
  long l, n;

  New(0, buf, p_num_samples, double);
  SAVEFREEPV(buf);
  n = ra_raw_get_unit(p_rec_handle, (unsigned int)p_channel, (size_t)p_start_sample, (size_t)p_num_samples, buf);

  arr = newAV();
  sv_2mortal((SV *)arr);
  for (l = 0; l < n; l++)
    av_store(arr, l, newSVnv(buf[l]));

  PUSHs(newRV_noinc((SV *)arr));


int
_if_meas_copy(p_meas_handle, p_dir)
  void * p_meas_handle
  char * p_dir
CODE:
  RETVAL = ra_meas_copy(p_meas_handle, p_dir);
OUTPUT:
  RETVAL

void *
_if_meas_move(p_meas_handle, p_dir)
  void * p_meas_handle
  char * p_dir
CODE:
  RETVAL = ra_meas_move(p_meas_handle, p_dir);
OUTPUT:
  RETVAL

int
_if_meas_delete(p_meas_handle)
  void * p_meas_handle
CODE:
  RETVAL = ra_meas_delete(p_meas_handle);
OUTPUT:
  RETVAL

void
_if_lib_use_plugin(p_ra_handle, p_index, p_use_it)
  void * p_ra_handle
  long p_index
  long p_use_it
CODE:
  ra_lib_use_plugin(p_ra_handle, p_index, p_use_it);

void *
_if_plugin_get_by_num(p_ra_handle, p_number, p_search_all)
  void * p_ra_handle
  long p_number
  long p_search_all
CODE:
  RETVAL = ra_plugin_get_by_num(p_ra_handle, p_number, p_search_all);
OUTPUT:
  RETVAL

void *
_if_plugin_get_by_name(p_ra_handle, p_name, p_search_all)
  void * p_ra_handle
  char * p_name
  long p_search_all
CODE:
  RETVAL = ra_plugin_get_by_name(p_ra_handle, p_name, p_search_all);
OUTPUT:
  RETVAL

void *
_if_proc_get(p_meas_handle, p_plugin_handle)
  void * p_meas_handle
  void * p_plugin_handle
CODE:
  struct proc_info *pi;

  pi = (struct proc_info *)ra_proc_get(p_meas_handle, p_plugin_handle, NULL);

  RETVAL = pi;
OUTPUT:
  RETVAL

int
_if_proc_do(p_process_handle)
  void * p_process_handle
CODE:
  int res;

  res = ra_proc_do(p_process_handle);

  RETVAL = res;
OUTPUT:
  RETVAL

long
_if_proc_get_result_idx(p_process_handle, p_res_ascii_id)
  void * p_process_handle
  char * p_res_ascii_id
CODE:
  long idx;

  idx = ra_proc_get_result_idx(p_process_handle, p_res_ascii_id);

  RETVAL = idx;
OUTPUT:
  RETVAL

void
_if_proc_get_result(p_process_handle, p_plugin_handle)
  void * p_process_handle
  void * p_plugin_handle
PPCODE:
  value_handle vh;
  int cnt;
  AV *all;
  long l, n_res;

  vh = ra_value_malloc();
  
  all = newAV();
  sv_2mortal((SV *)all);

  n_res = 0;
  if (ra_info_get(p_plugin_handle, RA_INFO_PL_NUM_RESULTS_L, vh) == 0)
    n_res = ra_value_get_long(vh);

  for (l = 0; l < n_res; l++)
  {
    AV *curr;

    if (ra_proc_get_result(p_process_handle, l, 0, vh) != 0)
    {
      av_store(all, l, &PL_sv_undef);
      continue;
    }
	
    curr = newAV();
    sv_2mortal((SV *)curr);

    if (ra_value_is_ok(vh))
        av_store(curr, 0, value_to_sv(vh));
    else
        av_store(curr, 0, &PL_sv_undef);

    av_store(curr, 1, newSVpv(ra_value_get_name(vh), strlen(ra_value_get_name(vh))));
    av_store(curr, 2, newSVpv(ra_value_get_desc(vh), strlen(ra_value_get_desc(vh))));

    av_store(all, l, newRV((SV *)curr));
  }
  ra_value_free(vh);

  PUSHs(newRV_noinc((SV *)all));


void
_if_proc_free(p_process_handle)
  void * p_process_handle
PPCODE:
  if (p_process_handle)
    ra_proc_free(p_process_handle);

int
_if_eval_save(p_meas_handle, p_file, p_use_ascii)
  void * p_meas_handle
  char * p_file
  long p_use_ascii
   CODE:
     RETVAL = ra_eval_save(p_meas_handle, p_file, p_use_ascii);
  OUTPUT:
  RETVAL

void
_if_eval_attribute_list(p_handle)
  void * p_handle
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_eval_attribute_list(p_handle, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
void
_if_eval_attribute_get(p_handle, p_attrib_name)
  void * p_handle
  const char * p_attrib_name
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_eval_attribute_get(p_handle, p_attrib_name, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
void
_if_eval_attribute_set(p_handle, p_attrib_name, p_attrib_type, p_attrib_value)
  void * p_handle
  char * p_attrib_name
  char * p_attrib_type
  void * p_attrib_value
  PPCODE:
    int ok = 1;
    value_handle vh;
    SV *value;

    value = (SV *)p_attrib_value;

    vh = ra_value_malloc();

    if (p_attrib_type[0] == 'p')  /* check if it's an array */
    {
      long len, l;
      AV *arr;
      SV **elem;

      if (SvROK(value) && SvTYPE(SvRV(value)) == SVt_PVAV)
        arr = (AV*)SvRV(value);
      else
        return;
      len = av_len(arr) + 1;

      if (p_attrib_type[1] == 'l')  /* long array? */
      {
        long *all = malloc(sizeof(long) * len);
        for (l = 0; l < len; l++)
        {
          elem = av_fetch(arr, l, 0);
          all[l] = (long)SvIV(*elem);
        }
        ra_value_set_long_array(vh, all, len);

        free(all);
      }
      else if (p_attrib_type[1] == 'd')  /* or double array? */
      {
        double *all = malloc(sizeof(double) * len);
        for (l = 0; l < len; l++)
        {
          elem = av_fetch(arr, l ,0);
          all[l] = (double)SvNV(*elem);
        }
        ra_value_set_double_array(vh, all, len);

        free(all);
      }
    }
    else if (p_attrib_type[0] == 'l') /* is it a long? */
    {
      long val = (long)SvIV(value);

      ra_value_set_long(vh, val);
    }
    else if (p_attrib_type[0] == 'd') /* is it a double? */
    {
      double val = (double)SvNV(value);

      ra_value_set_double(vh, val);
    }
    else if (p_attrib_type[0] == 'c') /* is it a string? */
    {
      STRLEN len;
      char *str;

      str = (char *)SvPV(value, len);

      ra_value_set_string(vh, str);
    }
    else
      ok = 0;

    if (ok)
      ra_eval_attribute_set(p_handle, p_attrib_name, vh);

    ra_value_free(vh);
  
  
long
_if_eval_attribute_unset(p_handle, p_attrib_name)
  void * p_handle
  const char * p_attrib_name
  CODE:
    RETVAL = ra_eval_attribute_unset(p_handle, p_attrib_name);
  OUTPUT:
  RETVAL

void *
_if_eval_add(p_meas_handle, p_name, p_desc, p_orig)
  void * p_meas_handle
  char * p_name
  char * p_desc
  long p_orig
  CODE:
    RETVAL = ra_eval_add(p_meas_handle, p_name, p_desc, p_orig);
  OUTPUT:
  RETVAL

long
_if_eval_delete(p_eval_handle)
  void * p_eval_handle
  CODE:
    RETVAL = ra_eval_delete(p_eval_handle);
  OUTPUT:
  RETVAL

void
_if_eval_get_all(p_meas_handle)
  void * p_meas_handle
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_eval_get_all(p_meas_handle, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
void *
_if_eval_get_original(p_meas_handle)
  void * p_meas_handle
  CODE:
    RETVAL = ra_eval_get_original(p_meas_handle);
  OUTPUT:
  RETVAL

void *
_if_eval_get_default(p_meas_handle)
  void * p_meas_handle
  CODE:
    RETVAL = ra_eval_get_default(p_meas_handle);
  OUTPUT:
  RETVAL

long
_if_eval_set_default(p_eval_handle)
  void * p_eval_handle
  CODE:
    RETVAL = ra_eval_set_default(p_eval_handle);
  OUTPUT:
  RETVAL

void *
_if_eval_get_handle(p_class_handle)
  void * p_class_handle
  CODE:
    RETVAL = ra_eval_get_handle(p_class_handle);
  OUTPUT:
  RETVAL

void *
_if_class_add(p_eval_handle, p_id, p_name, p_desc)
  void * p_eval_handle
  char * p_id
  char * p_name
  char * p_desc
  CODE:
    RETVAL = ra_class_add(p_eval_handle, p_id, p_name, p_desc);
  OUTPUT:
  RETVAL

void *
_if_class_add_predef(p_eval_handle, p_id)
  void * p_eval_handle
  char * p_id
  CODE:
    RETVAL = ra_class_add_predef(p_eval_handle, p_id);
  OUTPUT:
  RETVAL

long
_if_class_delete(p_class_handle)
  void * p_class_handle
  CODE:
    RETVAL = ra_class_delete(p_class_handle);
  OUTPUT:
  RETVAL

void
_if_class_get(p_eval_handle, p_id)
  void * p_eval_handle
  char * p_id
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_class_get(p_eval_handle, p_id, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
long
_if_class_add_event(p_class_handle, p_start_pos, p_end_pos)
  void * p_class_handle
  long p_start_pos
  long p_end_pos
  CODE:
    RETVAL = ra_class_add_event(p_class_handle, p_start_pos, p_end_pos);
  OUTPUT:
  RETVAL

long
_if_class_del_event(p_class_handle, p_event_id)
  void * p_class_handle
  long p_event_id
  CODE:
    RETVAL = ra_class_del_event(p_class_handle, p_event_id);
  OUTPUT:
  RETVAL

void
_if_class_get_event_pos(p_class_handle, p_event_id)
  void * p_class_handle
  long p_event_id
    PPCODE:
      value_handle vh;
      SV *value;
      long pos[2];

      if (ra_class_get_event_pos(p_class_handle, p_event_id, &(pos[0]), &(pos[1])) != 0)
      {
	PUSHs(&PL_sv_undef);
        return;
      }

      vh = ra_value_malloc();
      ra_value_set_long_array(vh, pos, 2);
      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
long
_if_class_set_event_pos(p_class_handle, p_event_id, p_start_pos, p_end_pos)
  void * p_class_handle
  long p_event_id
  long p_start_pos
  long p_end_pos
  CODE:
    RETVAL = ra_class_set_event_pos(p_class_handle, p_event_id, p_start_pos, p_end_pos);
  OUTPUT:
  RETVAL

void
_if_class_get_events(p_class_handle, p_start_pos, p_end_pos, p_complete, p_sort)
  void * p_class_handle
  long p_start_pos
  long p_end_pos
  long p_complete
  long p_sort
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_class_get_events(p_class_handle, p_start_pos, p_end_pos, p_complete, p_sort, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
long
_if_class_get_prev_event(p_class_handle, p_event_id)
  void * p_class_handle
  long p_event_id
  CODE:
    RETVAL = ra_class_get_prev_event(p_class_handle, p_event_id);
  OUTPUT:
  RETVAL

long
_if_class_get_next_event(p_class_handle, p_event_id)
  void * p_class_handle
  long p_event_id
  CODE:
    RETVAL = ra_class_get_next_event(p_class_handle, p_event_id);
  OUTPUT:
  RETVAL

void *
_if_class_get_handle(p_handle)
  void * p_handle
  CODE:
    RETVAL = ra_class_get_handle(p_handle);
  OUTPUT:
  RETVAL

void *
_if_prop_add(p_class_handle, p_id, p_value_type, p_name, p_desc, p_unit, p_use_minmax, p_min, p_max, p_has_ignore_value, p_ignore_value)
  void * p_class_handle
  char * p_id
  long p_value_type
  char * p_name
  char * p_desc
  char * p_unit
  long p_use_minmax
  double p_min
  double p_max
  long p_has_ignore_value
  double p_ignore_value
  CODE:
    RETVAL = ra_prop_add(p_class_handle, p_id, p_value_type,
                         p_name, p_desc, p_unit, p_use_minmax, p_min, p_max,
			 p_has_ignore_value, p_ignore_value);
  OUTPUT:
  RETVAL

void *
_if_prop_add_predef(p_class_handle, p_id)
  void * p_class_handle
  char * p_id
  CODE:
    RETVAL = ra_prop_add_predef(p_class_handle, p_id);
  OUTPUT:
  RETVAL

long
_if_prop_delete(p_prop_handle)
  void * p_prop_handle
  CODE:
    RETVAL = ra_prop_delete(p_prop_handle);
  OUTPUT:
  RETVAL

void *
_if_prop_get(p_class_handle, p_id)
  void * p_class_handle
  char * p_id
    CODE:
      RETVAL = ra_prop_get(p_class_handle, p_id);
  OUTPUT:
  RETVAL

void
_if_prop_get_all(p_class_handle)
  void * p_class_handle
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_prop_get_all(p_class_handle, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
long
_if_prop_set_value(p_prop_handle, p_event_id, p_ch, p_value_type, p_value)
  void * p_prop_handle
  long p_event_id
  long p_ch
  char * p_value_type
  SV * p_value
CODE:
  value_handle vh;
  int ret;

  vh = ra_value_malloc();

  ret = sv_to_value(p_value, p_value_type, vh);  
  if (ret == 0)
    ra_prop_set_value(p_prop_handle, p_event_id, p_ch, vh);

  ra_value_free(vh);

  RETVAL = ret;
OUTPUT:
  RETVAL

long
_if_prop_set_value_mass(p_prop_handle, p_event_id, p_ch, p_value_type, p_value)
  SV * p_prop_handle
  SV * p_event_id
  SV * p_ch
  char * p_value_type
  SV * p_value
CODE:
  value_handle vh, vh_id, vh_ch;
  int ret;

  vh_id = ra_value_malloc();
  vh_ch = ra_value_malloc();
  vh = ra_value_malloc();

  sv_to_value(p_event_id, "pl", vh_id);
  sv_to_value(p_ch, "pl", vh_ch);
  sv_to_value(p_event_id, p_value_type, vh);

  ret = ra_prop_set_value_mass(p_prop_handle, ra_value_get_long_array(vh_id),
                                 ra_value_get_long_array(vh_ch), vh);

  ra_value_free(vh_id);
  ra_value_free(vh_ch);
  ra_value_free(vh);

  RETVAL = ret;
OUTPUT:
  RETVAL

void
_if_prop_get_ch(p_prop_handle, p_id)
  void * p_prop_handle
  long p_id
    PPCODE:
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      if (ra_prop_get_ch(p_prop_handle, p_id, vh) != 0)
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);


void
_if_prop_get_value(p_prop_handle, p_id, p_ch)
  void * p_prop_handle
  long p_id
  long p_ch
PPCODE:
      value_handle vh;
      SV *value;
      long ret;

      vh = ra_value_malloc();
      ret = ra_prop_get_value(p_prop_handle, p_id, p_ch, vh);
      if ((ret != 0) && (ret != RA_WARN_CH_INDEP_PROP_VALUE))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }
      if (!ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);


void
_if_prop_get_value_all(p_prop_handle)
  void * p_prop_handle
    PPCODE:
      value_handle vh, vh_id, vh_ch;
      AV *all;

      all = newAV();
      sv_2mortal((SV *)all);

      vh = ra_value_malloc();
      vh_id = ra_value_malloc();
      vh_ch = ra_value_malloc();
      if (ra_prop_get_value_all(p_prop_handle, vh_id, vh_ch, vh) != 0)
      {
        ra_value_free(vh);
        ra_value_free(vh_id);
        ra_value_free(vh_ch);

	av_store(all, 0, &PL_sv_undef);
	av_store(all, 1, &PL_sv_undef);
	av_store(all, 2, &PL_sv_undef);
	PUSHs(newRV_noinc((SV *)all));

        return;
      }
      if (!ra_value_is_ok(vh) || !ra_value_is_ok(vh_id) || !ra_value_is_ok(vh_ch))
      {
        ra_value_free(vh);
        ra_value_free(vh_id);
        ra_value_free(vh_ch);

	av_store(all, 0, &PL_sv_undef);
	av_store(all, 1, &PL_sv_undef);
	av_store(all, 2, &PL_sv_undef);
	PUSHs(newRV_noinc((SV *)all));

        return;
      }

      av_store(all, 0, value_to_sv(vh_id));
      av_store(all, 1, value_to_sv(vh_ch));
      av_store(all, 2, value_to_sv(vh));

      ra_value_free(vh);
      ra_value_free(vh_id);
      ra_value_free(vh_ch);

      PUSHs(newRV_noinc((SV *)all));


void
_if_prop_get_events(p_prop_handle, p_val_type, p_min, p_max, p_ch)
  void * p_prop_handle
  char * p_val_type
  void * p_min
  void * p_max
  long p_ch
    PPCODE:
      int ret;
      value_handle vh, vh_min, vh_max;
      SV *ids;

      vh_min = ra_value_malloc();
      vh_max = ra_value_malloc();
      ret = sv_to_value((SV *)p_min, p_val_type, vh_min);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
	PUSHs(&PL_sv_undef);
	return;
      }
      ret = sv_to_value((SV *)p_max, p_val_type, vh_max);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
	PUSHs(&PL_sv_undef);
	return;
      }

      vh = ra_value_malloc();
      ret = ra_prop_get_events(p_prop_handle, vh_min, vh_max, p_ch, vh);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
	return;
      }

      ids = value_to_sv(vh);
      
      ra_value_free(vh_min);
      ra_value_free(vh_max);
      ra_value_free(vh);

      PUSHs(ids);


void *
_if_sum_add(p_class_handle, p_id, p_name, p_desc, p_num_dim, p_dim_name, p_dim_unit)
  void * p_class_handle
  char * p_id
  char * p_name
  char * p_desc
  long p_num_dim
  void * p_dim_name
  void * p_dim_unit
  CODE:
    RETVAL = ra_sum_add(p_class_handle, p_id, p_name, p_desc, 
                        p_num_dim, p_dim_name, p_dim_unit);
  OUTPUT:
  RETVAL

long
_if_sum_add_ch(p_sum_handle, p_ch, p_fid_offset)
  void * p_sum_handle
  long p_ch
  long p_fid_offset
  CODE:
    RETVAL = ra_sum_add_ch(p_sum_handle, p_ch, p_fid_offset);
  OUTPUT:
  RETVAL

long
_if_sum_delete(p_sum_handle)
  void * p_sum_handle
  CODE:
    RETVAL = ra_sum_delete(p_sum_handle);
  OUTPUT:
  RETVAL

void
_if_sum_get(p_class_handle, p_id)
  void * p_class_handle
  char * p_id
    PPCODE:
      value_handle vh;
      int ret;
      SV *value;

      vh = ra_value_malloc();

      ret = ra_sum_get(p_class_handle, p_id, vh);
      if ((ret != 0) || !ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
	return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
        
  
long
_if_sum_add_part(p_sum_handle, p_events)
  void * p_sum_handle
  void * p_events
  CODE:
    int ret;
    value_handle vh;

    vh = ra_value_malloc();

    if ((ret = sv_to_value((SV *)p_events, "l", vh)) == 0)
      RETVAL = ra_sum_add_part(p_sum_handle, ra_value_get_num_elem(vh), ra_value_get_long_array(vh));
    else
      RETVAL = ret;

    ra_value_free(vh);
  OUTPUT:
  RETVAL

long
_if_sum_del_part(p_sum_handle, p_part_id)
  void * p_sum_handle
  long p_part_id
  CODE:
    RETVAL = ra_sum_del_part(p_sum_handle, p_part_id);
  OUTPUT:
  RETVAL

void
_if_sum_get_part_all(p_sum_handle)
  void * p_sum_handle
    PPCODE:
      int ret;
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      ret = ra_sum_get_part_all(p_sum_handle, vh);
      if ((ret != 0) || !ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);
  
  
long
_if_sum_set_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim, p_data_type, p_data)
  void * p_sum_handle
  long p_part_id
  long p_ch_idx
  long p_dim
  char * p_data_type
  void * p_data
CODE:
  value_handle vh;
  int ret;

  vh = ra_value_malloc();

  ret = sv_to_value((SV *)p_data, p_data_type, vh);  
  if (ret == 0)
    ra_sum_set_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim, vh);

  ra_value_free(vh);

  RETVAL = ret;
OUTPUT:
  RETVAL

void
_if_sum_get_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim)
  void * p_sum_handle
  long p_part_id
  long p_ch_idx
  long p_dim
PPCODE:
      int ret;
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      ret = ra_sum_get_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim, vh);
      if ((ret != 0) || !ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);


void
_if_sum_get_part_events(p_sum_handle, p_part_id)
  void * p_sum_handle
  long p_part_id
    PPCODE:
      int ret;
      value_handle vh;
      SV *value;

      vh = ra_value_malloc();
      ret = ra_sum_get_part_events(p_sum_handle, p_part_id, vh);
      if ((ret != 0) || !ra_value_is_ok(vh))
      {
        ra_value_free(vh);
	PUSHs(&PL_sv_undef);
        return;
      }

      value = value_to_sv(vh);

      ra_value_free(vh);

      PUSHs(value);


void
_if_sum_set_part_events(p_sum_handle, p_part_id, p_event_ids)
  void * p_sum_handle
  long p_part_id
  SV * p_event_ids
PPCODE:
  int ret;
  value_handle vh;

  vh = ra_value_malloc();

  ret = sv_to_value(p_event_ids, "l", vh);
  if (ret == 0)
    ret = ra_sum_set_part_events(p_sum_handle, p_part_id, vh);

  ra_value_free(vh);

  PUSHs(sv_2mortal(newSViv(ret)));


