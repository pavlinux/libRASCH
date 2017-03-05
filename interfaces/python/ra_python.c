/*
 * Don't change this file. It was created automatically and every change
 * you made will be lost after the next make.
 *
 * created using make_interfaces.pl Version 0.2.5  15.09.2008

 */

#include <Python.h>
#include <ra.h>
#include "ra_python.h"

static PyObject *
if_ra_lib_init(PyObject *self, PyObject *args)
{

    ra_handle ra;
  

  if (!PyArg_ParseTuple(args, "", NULL))
    return NULL;


    ra = ra_lib_init();
    return Py_BuildValue("l", ra);
  } /* if_ra_lib_init() */

static PyObject *
if_ra_lib_close(PyObject *self, PyObject *args)
{
  void * p_ra_handle;

  if (!PyArg_ParseTuple(args, "l", &p_ra_handle))
    return NULL;


    ra_lib_close(p_ra_handle);
  
    Py_INCREF(Py_None);
    return Py_None;
  } /* if_ra_lib_close() */

static PyObject *
if_ra_lib_get_error(PyObject *self, PyObject *args)
{

  long err_num;
  char err_text[200];
  PyObject *list = PyList_New(2);

  void * p_ra_handle;

  if (!PyArg_ParseTuple(args, "l", &p_ra_handle))
    return NULL;


  err_num = ra_lib_get_error(p_ra_handle, err_text, 200);

  PyList_SetItem(list, 0, Py_BuildValue("l", err_num));
  PyList_SetItem(list, 1, Py_BuildValue("s", err_text));

  return list;
} /* if_ra_lib_get_error() */

static PyObject *
if_ra_lib_handle_from_any_handle(PyObject *self, PyObject *args)
{

  ra_handle ra;

  void * p_any_handle;

  if (!PyArg_ParseTuple(args, "l", &p_any_handle))
    return NULL;


  ra = ra_lib_handle_from_any_handle(p_any_handle);
  return Py_BuildValue("l", ra);
} /* if_ra_lib_handle_from_any_handle() */

static PyObject *
if_ra_lib_set_option(PyObject *self, PyObject *args)
{

  value_handle vh;
  int ret;

  void * p_handle;
  char * p_option_name;
  char * p_option_type;
  PyObject * p_option_value;

  if (!PyArg_ParseTuple(args, "lssO", &p_handle, &p_option_name, &p_option_type, &p_option_value))
    return NULL;


  vh = ra_value_malloc();

  ret = py_object_to_value(p_option_value, p_option_type, vh);
  if (ret == 0)
    ret = ra_lib_set_option(p_handle, p_option_name, vh);

  ra_value_free(vh);

  return Py_BuildValue("l", (long)ret);

} /* if_ra_lib_set_option() */

static PyObject *
if_ra_lib_get_option(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *value;

  void * p_handle;
  char * p_option_name;

  if (!PyArg_ParseTuple(args, "ls", &p_handle, &p_option_name))
    return NULL;


    vh = ra_value_malloc();

    if (ra_lib_get_option(p_handle, p_option_name, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;

} /* if_ra_lib_get_option() */

static PyObject *
if_ra_lib_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_ra_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_ra_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_ra_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_lib_info_get() */

static PyObject *
if_ra_plugin_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_plugin_handle;
  long p_num;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_plugin_handle, &p_num, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_num);
  ra_info_get_by_name(p_plugin_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_plugin_info_get() */

static PyObject *
if_ra_result_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_plugin_handle;
  long p_result_num;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_plugin_handle, &p_result_num, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_result_num);
  ra_info_get_by_name(p_plugin_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_result_info_get() */

static PyObject *
if_ra_meas_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_meas_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_meas_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_meas_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_meas_info_get() */

static PyObject *
if_ra_obj_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_meas_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_meas_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_meas_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_obj_info_get() */

static PyObject *
if_ra_rec_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_rec_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_rec_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_rec_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_rec_info_get() */

static PyObject *
if_ra_dev_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_rec_handle;
  long p_dev_num;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_rec_handle, &p_dev_num, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_dev_num);
  ra_info_get_by_name(p_rec_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_dev_info_get() */

static PyObject *
if_ra_ch_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_rec_handle;
  long p_channel;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_rec_handle, &p_channel, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_channel);
  ra_info_get_by_name(p_rec_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_ch_info_get() */

static PyObject *
if_ra_eval_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_eval_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_eval_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_eval_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_eval_info_get() */

static PyObject *
if_ra_class_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_class_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_class_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_class_handle, p_info_name, vh);
  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_class_info_get() */

static PyObject *
if_ra_prop_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_prop_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_prop_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_prop_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_prop_info_get() */

static PyObject *
if_ra_summary_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_sum_handle;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "ls", &p_sum_handle, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_info_get_by_name(p_sum_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_summary_info_get() */

static PyObject *
if_ra_sum_dim_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_sum_handle;
  long p_dim;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_sum_handle, &p_dim, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_dim);
  ra_info_get_by_name(p_sum_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_sum_dim_info_get() */

static PyObject *
if_ra_sum_ch_info_get(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *ret;

  void * p_sum_handle;
  long p_channel;
  char * p_info_name;

  if (!PyArg_ParseTuple(args, "lls", &p_sum_handle, &p_channel, &p_info_name))
    return NULL;


  vh = ra_value_malloc();
  ra_value_set_number(vh, p_channel);
  ra_info_get_by_name(p_sum_handle, p_info_name, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;
} /* if_ra_sum_ch_info_get() */

static PyObject *
if_ra_info_get_by_idx(PyObject *self, PyObject *args)
{

  value_handle vh;
  long inf_type;
  PyObject *ret;

  void * p_any_handle;
  char * p_info_type;
  long p_index;

  if (!PyArg_ParseTuple(args, "lsl", &p_any_handle, &p_info_type, &p_index))
    return NULL;


  inf_type = get_info_type(p_info_type);
  if (inf_type < 0)
    return 0;  

  vh = ra_value_malloc();

  ra_info_get_by_idx(p_any_handle, inf_type, p_index, vh);

  ret = value_to_py_list(vh);

  ra_value_free(vh);

  return ret;

} /* if_ra_info_get_by_idx() */

static PyObject *
if_ra_info_set(PyObject *self, PyObject *args)
{

  value_handle vh;
  int ret;

  void * p_handle;
  char * p_info_name;
  char * p_info_type;
  PyObject * p_info_value;

  if (!PyArg_ParseTuple(args, "lssO", &p_handle, &p_info_name, &p_info_type, &p_info_value))
    return NULL;


  vh = ra_value_malloc();

  ret = py_object_to_value(p_info_value, p_info_type, vh);
  if (ret == 0)
    ret = ra_info_set_by_name(p_handle, p_info_name, vh);

  ra_value_free(vh);

  return Py_BuildValue("l", (long)ret);
} /* if_ra_info_set() */

static PyObject *
if_ra_meas_handle_from_any_handle(PyObject *self, PyObject *args)
{

  meas_handle meas;

  void * p_any_handle;

  if (!PyArg_ParseTuple(args, "l", &p_any_handle))
    return NULL;


  meas = ra_meas_handle_from_any_handle(p_any_handle);
  return Py_BuildValue("l", meas);
} /* if_ra_meas_handle_from_any_handle() */

static PyObject *
if_ra_meas_find(PyObject *self, PyObject *args)
{

    char **sigs_found = NULL;
    char **plugins = NULL;
    long n_sigs = 0;
    struct ra_find_struct rfs;
    ra_find_handle mfh;
    long l;
    PyObject *list_fn;
    PyObject *list_pl;
    PyObject *list_ret;    

  void * p_ra_handle;
  char * p_directory;

  if (!PyArg_ParseTuple(args, "ls", &p_ra_handle, &p_directory))
    return NULL;


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

    list_fn = PyList_New(n_sigs);
    list_pl = PyList_New(n_sigs);
    for (l = 0; l < n_sigs; l++)
    {
        PyList_SetItem(list_fn, l, Py_BuildValue("s", sigs_found[l]));
	free(sigs_found[l]);

        PyList_SetItem(list_pl, l, Py_BuildValue("s", plugins[l]));
	free(plugins[l]);
    }
    free(sigs_found);
    free(plugins);

    list_ret = PyList_New(2);
    PyList_SetItem(list_ret, 0, Py_BuildValue("O", list_fn));
    PyList_SetItem(list_ret, 1, Py_BuildValue("O", list_pl));

    return list_ret;

} /* if_ra_meas_find() */

static PyObject *
if_ra_meas_open(PyObject *self, PyObject *args)
{

  meas_handle meas;

  void * p_ra_handle;
  char * p_signal_name;
  char * p_eval_file;
  long p_open_fast;

  if (!PyArg_ParseTuple(args, "lssl", &p_ra_handle, &p_signal_name, &p_eval_file, &p_open_fast))
    return NULL;


  meas = ra_meas_open(p_ra_handle, p_signal_name, p_eval_file, p_open_fast);
  return Py_BuildValue("l", meas);
} /* if_ra_meas_open() */

static PyObject *
if_ra_meas_new(PyObject *self, PyObject *args)
{

  meas_handle meas;

  void * p_ra_handle;
  char * p_dir_name;
  char * p_signal_name;

  if (!PyArg_ParseTuple(args, "lss", &p_ra_handle, &p_dir_name, &p_signal_name))
    return NULL;


  meas = ra_meas_new(p_ra_handle, p_dir_name, p_signal_name);
  return Py_BuildValue("l", meas);
} /* if_ra_meas_new() */

static PyObject *
if_ra_meas_save(PyObject *self, PyObject *args)
{
  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


  ra_meas_save(p_meas_handle);

  Py_INCREF(Py_None);
  return Py_None;
} /* if_ra_meas_save() */

static PyObject *
if_ra_meas_close(PyObject *self, PyObject *args)
{
  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


  ra_meas_close(p_meas_handle);

  Py_INCREF(Py_None);
  return Py_None;
} /* if_ra_meas_close() */

static PyObject *
if_ra_rec_get_first(PyObject *self, PyObject *args)
{

  rec_handle rec;

  void * p_meas_handle;
  long p_session_number;

  if (!PyArg_ParseTuple(args, "ll", &p_meas_handle, &p_session_number))
    return NULL;


  rec = ra_rec_get_first(p_meas_handle, p_session_number);
  return Py_BuildValue("l", rec);
} /* if_ra_rec_get_first() */

static PyObject *
if_ra_rec_get_next(PyObject *self, PyObject *args)
{

  rec_handle rec;

  void * p_rec_handle;

  if (!PyArg_ParseTuple(args, "l", &p_rec_handle))
    return NULL;


  rec = ra_rec_get_next(p_rec_handle);
  return Py_BuildValue("l", rec);
} /* if_ra_rec_get_next() */

static PyObject *
if_ra_rec_get_first_child(PyObject *self, PyObject *args)
{

  rec_handle rec;

  void * p_rec_handle;

  if (!PyArg_ParseTuple(args, "l", &p_rec_handle))
    return NULL;


  rec = ra_rec_get_first_child(p_rec_handle);
  return Py_BuildValue("l", rec);
} /* if_ra_rec_get_first_child() */

static PyObject *
if_ra_raw_get(PyObject *self, PyObject *args)
{

  DWORD *buf_raw = NULL;
  long l, n;
  PyObject *list;

  void * p_rec_handle;
  long p_channel;
  long p_start_sample;
  long p_num_samples;

  if (!PyArg_ParseTuple(args, "llll", &p_rec_handle, &p_channel, &p_start_sample, &p_num_samples))
    return NULL;


  buf_raw = malloc(sizeof(DWORD) * p_num_samples);
  n = ra_raw_get(p_rec_handle, (unsigned int)p_channel, (size_t)p_start_sample, (size_t)p_num_samples, buf_raw, NULL);

  list = PyList_New(n);
  for (l = 0; l < n; l++)
    PyList_SetItem(list, l, Py_BuildValue("l", buf_raw[l]));

  free(buf_raw);

  return list;

} /* if_ra_raw_get() */

static PyObject *
if_ra_raw_get_unit(PyObject *self, PyObject *args)
{

  double *buf = NULL;
  long l, n;
  PyObject *list;

  void * p_rec_handle;
  long p_channel;
  long p_start_sample;
  long p_num_samples;

  if (!PyArg_ParseTuple(args, "llll", &p_rec_handle, &p_channel, &p_start_sample, &p_num_samples))
    return NULL;


  buf = malloc(sizeof(double) * p_num_samples);
  n = ra_raw_get_unit(p_rec_handle, (unsigned int)p_channel, (size_t)p_start_sample, (size_t)p_num_samples, buf);

  list = PyList_New(n);

  for (l = 0; l < n; l++)
    PyList_SetItem(list, l, Py_BuildValue("d", buf[l]));

  free(buf);


  return list;

} /* if_ra_raw_get_unit() */

static PyObject *
if_ra_meas_copy(PyObject *self, PyObject *args)
{

  long ok;

  void * p_meas_handle;
  char * p_dir;

  if (!PyArg_ParseTuple(args, "ls", &p_meas_handle, &p_dir))
    return NULL;


  ok = (long)ra_meas_copy(p_meas_handle, p_dir);
  return Py_BuildValue("l", ok);
} /* if_ra_meas_copy() */

static PyObject *
if_ra_meas_move(PyObject *self, PyObject *args)
{

  meas_handle mh_moved;

  void * p_meas_handle;
  char * p_dir;

  if (!PyArg_ParseTuple(args, "ls", &p_meas_handle, &p_dir))
    return NULL;


  mh_moved = ra_meas_move(p_meas_handle, p_dir);
  return Py_BuildValue("l", mh_moved);
} /* if_ra_meas_move() */

static PyObject *
if_ra_meas_delete(PyObject *self, PyObject *args)
{

  long ok;

  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


  ok = (long)ra_meas_delete(p_meas_handle);
  return Py_BuildValue("l", ok);
} /* if_ra_meas_delete() */

static PyObject *
if_ra_lib_use_plugin(PyObject *self, PyObject *args)
{
  void * p_ra_handle;
  long p_index;
  long p_use_it;

  if (!PyArg_ParseTuple(args, "lll", &p_ra_handle, &p_index, &p_use_it))
    return NULL;


  ra_lib_use_plugin(p_ra_handle, p_index, p_use_it);

  Py_INCREF(Py_None);
  return Py_None;
} /* if_ra_lib_use_plugin() */

static PyObject *
if_ra_plugin_get_by_num(PyObject *self, PyObject *args)
{

  plugin_handle pl;

  void * p_ra_handle;
  long p_number;
  long p_search_all;

  if (!PyArg_ParseTuple(args, "lll", &p_ra_handle, &p_number, &p_search_all))
    return NULL;


  pl = ra_plugin_get_by_num(p_ra_handle, p_number, p_search_all);
  return Py_BuildValue("l", pl);
} /* if_ra_plugin_get_by_num() */

static PyObject *
if_ra_plugin_get_by_name(PyObject *self, PyObject *args)
{

  plugin_handle pl;

  void * p_ra_handle;
  char * p_name;
  long p_search_all;

  if (!PyArg_ParseTuple(args, "lsl", &p_ra_handle, &p_name, &p_search_all))
    return NULL;


  pl = ra_plugin_get_by_name(p_ra_handle, p_name, p_search_all);
  return Py_BuildValue("l", pl);
} /* if_ra_plugin_get_by_name() */

static PyObject *
if_ra_proc_get(PyObject *self, PyObject *args)
{

  struct proc_info *pi;

  void * p_meas_handle;
  void * p_plugin_handle;

  if (!PyArg_ParseTuple(args, "ll", &p_meas_handle, &p_plugin_handle))
    return NULL;


  pi = (struct proc_info *)ra_proc_get(p_meas_handle, p_plugin_handle, NULL);

  return Py_BuildValue("l", pi);
} /* if_ra_proc_get() */

static PyObject *
if_ra_proc_do(PyObject *self, PyObject *args)
{

  int res;

  void * p_process_handle;

  if (!PyArg_ParseTuple(args, "l", &p_process_handle))
    return NULL;


  res = ra_proc_do(p_process_handle);

  return Py_BuildValue("l", res);
} /* if_ra_proc_do() */

static PyObject *
if_ra_proc_get_result_idx(PyObject *self, PyObject *args)
{

  long idx;

  void * p_process_handle;
  char * p_res_ascii_id;

  if (!PyArg_ParseTuple(args, "ls", &p_process_handle, &p_res_ascii_id))
    return NULL;


  idx = ra_proc_get_result_idx(p_process_handle, p_res_ascii_id);

  return Py_BuildValue("l", idx);
} /* if_ra_proc_get_result_idx() */

static PyObject *
if_ra_proc_get_result(PyObject *self, PyObject *args)
{

  value_handle vh;
  PyObject *list;
  long l, n_res;

  void * p_process_handle;
  void * p_plugin_handle;

  if (!PyArg_ParseTuple(args, "ll", &p_process_handle, &p_plugin_handle))
    return NULL;


  vh = ra_value_malloc();

  n_res = 0;
  if (ra_info_get(p_plugin_handle, RA_INFO_PL_NUM_RESULTS_L, vh) == 0)
    n_res = ra_value_get_long(vh);

  list = PyList_New(n_res);
  for (l = 0; l < n_res; l++)
  {
    PyObject *curr;

    curr = PyList_New(3);

    if (ra_proc_get_result(p_process_handle, l, 0, vh) == 0)
    {
      PyList_SetItem(curr, 0, value_to_py_object(vh));
      PyList_SetItem(curr, 1, Py_BuildValue("s", ra_value_get_name(vh)));
      PyList_SetItem(curr, 2, Py_BuildValue("s", ra_value_get_desc(vh)));
    }
    else
    {
      PyList_SetItem(curr, 0, Py_BuildValue(""));
      PyList_SetItem(curr, 1, Py_BuildValue(""));
      PyList_SetItem(curr, 2, Py_BuildValue(""));
    }

    PyList_SetItem(list, l, curr);
  }
  ra_value_free(vh);

  return list;

} /* if_ra_proc_get_result() */

static PyObject *
if_ra_proc_free(PyObject *self, PyObject *args)
{
  void * p_process_handle;

  if (!PyArg_ParseTuple(args, "l", &p_process_handle))
    return NULL;


  if (p_process_handle)
    ra_proc_free(p_process_handle);

  Py_INCREF(Py_None);
  return Py_None;
} /* if_ra_proc_free() */

static PyObject *
if_ra_eval_save(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_meas_handle;
  char * p_file;
  long p_use_ascii;

  if (!PyArg_ParseTuple(args, "lsl", &p_meas_handle, &p_file, &p_use_ascii))
    return NULL;


    ok = (long)ra_eval_save(p_meas_handle, p_file, p_use_ascii);
    return Py_BuildValue("l", ok);  
  } /* if_ra_eval_save() */

static PyObject *
if_ra_eval_attribute_list(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_handle;

  if (!PyArg_ParseTuple(args, "l", &p_handle))
    return NULL;


    vh = ra_value_malloc();

    if (ra_eval_attribute_list(p_handle, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_eval_attribute_list() */

static PyObject *
if_ra_eval_attribute_get(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_handle;
  const char * p_attrib_name;

  if (!PyArg_ParseTuple(args, "l", &p_handle, &p_attrib_name))
    return NULL;


    vh = ra_value_malloc();

    if (ra_eval_attribute_get(p_handle, p_attrib_name, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_eval_attribute_get() */

static PyObject *
if_ra_eval_attribute_set(PyObject *self, PyObject *args)
{

    int ok = 1;
    value_handle vh;
    PyObject *value;
  
  void * p_handle;
  char * p_attrib_name;
  char * p_attrib_type;
  void * p_attrib_value;

  if (!PyArg_ParseTuple(args, "lssl", &p_handle, &p_attrib_name, &p_attrib_type, &p_attrib_value))
    return NULL;


    value = (PyObject *)p_attrib_value;

    vh = ra_value_malloc();

    if (p_attrib_type[0] == 'p')  /* check if it's an array */
    {
      int len, i;
      PyObject *item;

      if (!PyList_Check(value))
        return Py_None;

      len = PyList_Size(value);

      if (p_attrib_type[1] == 'l')  /* long array? */
      {
        long *all = malloc(sizeof(long) * len);
        for (i = 0; i < len; i++)
        {
          item = PyList_GetItem(value, i);
	  PyArg_ParseTuple(item, "l", &(all[i]));
        }
        ra_value_set_long_array(vh, all, len);

        free(all);
      }
      else if (p_attrib_type[1] == 'd')  /* or double array? */
      {
        double *all = malloc(sizeof(double) * len);
        for (i = 0; i < len; i++)
        {
          item = PyList_GetItem(value, i);
	  PyArg_ParseTuple(item, "d", &(all[i]));
        }
        ra_value_set_double_array(vh, all, len);

        free(all);
      }
    }
    else if (p_attrib_type[0] == 'l') /* is it a long? */
    {
      long val;

      PyArg_ParseTuple(value, "l", &val);
      ra_value_set_long(vh, val);
    }
    else if (p_attrib_type[0] == 'd') /* is it a double? */
    {
      double val;

      PyArg_ParseTuple(value, "d", &val);
      ra_value_set_double(vh, val);
    }
    else if (p_attrib_type[0] == 'c') /* is it a string? */
    {
      char *str;

      PyArg_ParseTuple(value, "s", &str);
      ra_value_set_string(vh, str);
    }
    else
      ok = 0;

    if (ok)
      ra_eval_attribute_set(p_handle, p_attrib_name, vh);

    ra_value_free(vh);

    return Py_None;
  
  } /* if_ra_eval_attribute_set() */

static PyObject *
if_ra_eval_attribute_unset(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_handle;
  const char * p_attrib_name;

  if (!PyArg_ParseTuple(args, "l", &p_handle, &p_attrib_name))
    return NULL;


    ok = (long)ra_eval_attribute_unset(p_handle, p_attrib_name);
    return Py_BuildValue("l", ok);  
  } /* if_ra_eval_attribute_unset() */

static PyObject *
if_ra_eval_add(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_meas_handle;
  char * p_name;
  char * p_desc;
  long p_orig;

  if (!PyArg_ParseTuple(args, "lssl", &p_meas_handle, &p_name, &p_desc, &p_orig))
    return NULL;


    eval = ra_eval_add(p_meas_handle, p_name, p_desc, p_orig);
    return Py_BuildValue("l", eval);
  } /* if_ra_eval_add() */

static PyObject *
if_ra_eval_delete(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_eval_handle;

  if (!PyArg_ParseTuple(args, "l", &p_eval_handle))
    return NULL;


    ok = (long)ra_eval_delete(p_eval_handle);
    return Py_BuildValue("l", ok);
  } /* if_ra_eval_delete() */

static PyObject *
if_ra_eval_get_all(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


    vh = ra_value_malloc();

    if (ra_eval_get_all(p_meas_handle, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_eval_get_all() */

static PyObject *
if_ra_eval_get_original(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


    eval = ra_eval_get_original(p_meas_handle);
    return Py_BuildValue("l", eval);
  } /* if_ra_eval_get_original() */

static PyObject *
if_ra_eval_get_default(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_meas_handle;

  if (!PyArg_ParseTuple(args, "l", &p_meas_handle))
    return NULL;


    eval = ra_eval_get_default(p_meas_handle);
    return Py_BuildValue("l", eval);
  } /* if_ra_eval_get_default() */

static PyObject *
if_ra_eval_set_default(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_eval_handle;

  if (!PyArg_ParseTuple(args, "l", &p_eval_handle))
    return NULL;


    ok = (long)ra_eval_set_default(p_eval_handle);
    return Py_BuildValue("l", ok);
  } /* if_ra_eval_set_default() */

static PyObject *
if_ra_eval_get_handle(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_class_handle;

  if (!PyArg_ParseTuple(args, "l", &p_class_handle))
    return NULL;


    eval = ra_eval_get_handle(p_class_handle);
    return Py_BuildValue("l", eval);
  } /* if_ra_eval_get_handle() */

static PyObject *
if_ra_class_add(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_eval_handle;
  char * p_id;
  char * p_name;
  char * p_desc;

  if (!PyArg_ParseTuple(args, "lsss", &p_eval_handle, &p_id, &p_name, &p_desc))
    return NULL;


    eval = ra_class_add(p_eval_handle, p_id, p_name, p_desc);
    return Py_BuildValue("l", eval);
  } /* if_ra_class_add() */

static PyObject *
if_ra_class_add_predef(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_eval_handle;
  char * p_id;

  if (!PyArg_ParseTuple(args, "ls", &p_eval_handle, &p_id))
    return NULL;


    eval = ra_class_add_predef(p_eval_handle, p_id);
    return Py_BuildValue("l", eval);
  } /* if_ra_class_add_predef() */

static PyObject *
if_ra_class_delete(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_class_handle;

  if (!PyArg_ParseTuple(args, "l", &p_class_handle))
    return NULL;


    ok = (long)ra_class_delete(p_class_handle);
    return Py_BuildValue("l", ok);
  } /* if_ra_class_delete() */

static PyObject *
if_ra_class_get(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_eval_handle;
  char * p_id;

  if (!PyArg_ParseTuple(args, "ls", &p_eval_handle, &p_id))
    return NULL;


    vh = ra_value_malloc();

    if (ra_class_get(p_eval_handle, p_id, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_class_get() */

static PyObject *
if_ra_class_add_event(PyObject *self, PyObject *args)
{

    long ev_id;
  
  void * p_class_handle;
  long p_start_pos;
  long p_end_pos;

  if (!PyArg_ParseTuple(args, "lll", &p_class_handle, &p_start_pos, &p_end_pos))
    return NULL;


    ev_id = ra_class_add_event(p_class_handle, p_start_pos, p_end_pos);
    return Py_BuildValue("l", ev_id);
  } /* if_ra_class_add_event() */

static PyObject *
if_ra_class_del_event(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_class_handle;
  long p_event_id;

  if (!PyArg_ParseTuple(args, "ll", &p_class_handle, &p_event_id))
    return NULL;


    ok = ra_class_del_event(p_class_handle, p_event_id);
    return Py_BuildValue("l", ok);
  } /* if_ra_class_del_event() */

static PyObject *
if_ra_class_get_event_pos(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
    long pos[2];
  
  void * p_class_handle;
  long p_event_id;

  if (!PyArg_ParseTuple(args, "ll", &p_class_handle, &p_event_id))
    return NULL;


    if (ra_class_get_event_pos(p_class_handle, p_event_id, &(pos[0]), &(pos[1])) != 0)
    {
        return Py_None;
    }

    vh = ra_value_malloc();
    ra_value_set_long_array(vh, pos, 2);
    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_class_get_event_pos() */

static PyObject *
if_ra_class_set_event_pos(PyObject *self, PyObject *args)
{

    long ret;
  
  void * p_class_handle;
  long p_event_id;
  long p_start_pos;
  long p_end_pos;

  if (!PyArg_ParseTuple(args, "llll", &p_class_handle, &p_event_id, &p_start_pos, &p_end_pos))
    return NULL;


    ret = ra_class_set_event_pos(p_class_handle, p_event_id, p_start_pos, p_end_pos);
    return Py_BuildValue("l", ret);
  } /* if_ra_class_set_event_pos() */

static PyObject *
if_ra_class_get_events(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_class_handle;
  long p_start_pos;
  long p_end_pos;
  long p_complete;
  long p_sort;

  if (!PyArg_ParseTuple(args, "lllll", &p_class_handle, &p_start_pos, &p_end_pos, &p_complete, &p_sort))
    return NULL;


    vh = ra_value_malloc();

    if (ra_class_get_events(p_class_handle, p_start_pos, p_end_pos, p_complete, p_sort, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_class_get_events() */

static PyObject *
if_ra_class_get_prev_event(PyObject *self, PyObject *args)
{

    long ev_id;
  
  void * p_class_handle;
  long p_event_id;

  if (!PyArg_ParseTuple(args, "ll", &p_class_handle, &p_event_id))
    return NULL;


    ev_id = ra_class_get_prev_event(p_class_handle, p_event_id);
    return Py_BuildValue("l", ev_id);
  } /* if_ra_class_get_prev_event() */

static PyObject *
if_ra_class_get_next_event(PyObject *self, PyObject *args)
{

    long ev_id;
  
  void * p_class_handle;
  long p_event_id;

  if (!PyArg_ParseTuple(args, "ll", &p_class_handle, &p_event_id))
    return NULL;


    ev_id = ra_class_get_next_event(p_class_handle, p_event_id);
    return Py_BuildValue("l", ev_id);
  } /* if_ra_class_get_next_event() */

static PyObject *
if_ra_class_get_handle(PyObject *self, PyObject *args)
{

    eval_handle eval;
  
  void * p_handle;

  if (!PyArg_ParseTuple(args, "l", &p_handle))
    return NULL;


    eval = ra_class_get_handle(p_handle);
    return Py_BuildValue("l", eval);
  } /* if_ra_class_get_handle() */

static PyObject *
if_ra_prop_add(PyObject *self, PyObject *args)
{

    prop_handle prop;
  
  void * p_class_handle;
  char * p_id;
  long p_value_type;
  char * p_name;
  char * p_desc;
  char * p_unit;
  long p_use_minmax;
  double p_min;
  double p_max;
  long p_has_ignore_value;
  double p_ignore_value;

  if (!PyArg_ParseTuple(args, "lslsssll", &p_class_handle, &p_id, &p_value_type, &p_name, &p_desc, &p_unit, &p_use_minmax, &p_min, &p_max, &p_has_ignore_value, &p_ignore_value))
    return NULL;


    prop = ra_prop_add(p_class_handle, p_id, p_value_type,
                       p_name, p_desc, p_unit, p_use_minmax, p_min, p_max,
		       p_has_ignore_value, p_ignore_value);
    return Py_BuildValue("l", prop);
  } /* if_ra_prop_add() */

static PyObject *
if_ra_prop_add_predef(PyObject *self, PyObject *args)
{

    prop_handle prop;
  
  void * p_class_handle;
  char * p_id;

  if (!PyArg_ParseTuple(args, "ls", &p_class_handle, &p_id))
    return NULL;


    prop = ra_prop_add_predef(p_class_handle, p_id);
    return Py_BuildValue("l", prop);
  } /* if_ra_prop_add_predef() */

static PyObject *
if_ra_prop_delete(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_prop_handle;

  if (!PyArg_ParseTuple(args, "l", &p_prop_handle))
    return NULL;


    ok = (long)ra_prop_delete(p_prop_handle);
    return Py_BuildValue("l", ok);
  } /* if_ra_prop_delete() */

static PyObject *
if_ra_prop_get(PyObject *self, PyObject *args)
{

    prop_handle prop;
  
  void * p_class_handle;
  char * p_id;

  if (!PyArg_ParseTuple(args, "ls", &p_class_handle, &p_id))
    return NULL;


    prop = ra_prop_get(p_class_handle, p_id);
    return Py_BuildValue("l", prop);
  } /* if_ra_prop_get() */

static PyObject *
if_ra_prop_get_all(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;
  
  void * p_class_handle;

  if (!PyArg_ParseTuple(args, "l", &p_class_handle))
    return NULL;


    vh = ra_value_malloc();

    if (ra_prop_get_all(p_class_handle, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_prop_get_all() */

static PyObject *
if_ra_prop_set_value(PyObject *self, PyObject *args)
{

  value_handle vh;
  int ret;

  void * p_prop_handle;
  long p_event_id;
  long p_ch;
  char * p_value_type;
  PyObject * p_value;

  if (!PyArg_ParseTuple(args, "lllsO", &p_prop_handle, &p_event_id, &p_ch, &p_value_type, &p_value))
    return NULL;


  vh = ra_value_malloc();

  ret = py_object_to_value(p_value, p_value_type, vh);
  if (ret == 0)
    ra_prop_set_value(p_prop_handle, p_event_id, p_ch, vh);

  ra_value_free(vh);

  return Py_BuildValue("l", ret);

} /* if_ra_prop_set_value() */

static PyObject *
if_ra_prop_set_value_mass(PyObject *self, PyObject *args)
{

  int ret;
  value_handle vh, vh_id, vh_ch;

  PyObject * p_prop_handle;
  PyObject * p_event_id;
  PyObject * p_ch;
  char * p_value_type;
  PyObject * p_value;

  if (!PyArg_ParseTuple(args, "OOOsO", &p_prop_handle, &p_event_id, &p_ch, &p_value_type, &p_value))
    return NULL;


  vh_id = ra_value_malloc();
  ret = py_object_to_value(p_event_id, "pl", vh_id);
  if (ret != 0)
  {
    ra_value_free(vh_id);
    return Py_BuildValue("l", ret);
  }

  vh_ch = ra_value_malloc();
  ret = py_object_to_value(p_ch, "pl", vh_ch);
  if (ret != 0)
  {
    ra_value_free(vh_id);
    ra_value_free(vh_ch);
    return Py_BuildValue("l", ret);
  }

  vh = ra_value_malloc();
  ret = py_object_to_value(p_event_id, p_value_type, vh);
  if (ret != 0)
  {
    ra_value_free(vh_id);
    ra_value_free(vh_ch);
    ra_value_free(vh);
    return Py_BuildValue("l", ret);
  }

  ret = ra_prop_set_value_mass(p_prop_handle, ra_value_get_long_array(vh_id),
                               ra_value_get_long_array(vh_ch), vh);

  ra_value_free(vh_id);
  ra_value_free(vh_ch);
  ra_value_free(vh);

  return Py_BuildValue("l", ret);

} /* if_ra_prop_set_value_mass() */

static PyObject *
if_ra_prop_get_ch(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;

  void * p_prop_handle;
  long p_id;

  if (!PyArg_ParseTuple(args, "ll", &p_prop_handle, &p_id))
    return NULL;


    vh = ra_value_malloc();

    if (ra_prop_get_ch(p_prop_handle, p_id, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;

} /* if_ra_prop_get_ch() */

static PyObject *
if_ra_prop_get_value(PyObject *self, PyObject *args)
{

    value_handle vh;
    PyObject *value;

  void * p_prop_handle;
  long p_id;
  long p_ch;

  if (!PyArg_ParseTuple(args, "lll", &p_prop_handle, &p_id, &p_ch))
    return NULL;


    vh = ra_value_malloc();

    if (ra_prop_get_value(p_prop_handle, p_id, p_ch, vh) != 0)
    {
        ra_value_free(vh);
        return Py_None;
    }
    if (!ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;

} /* if_ra_prop_get_value() */

static PyObject *
if_ra_prop_get_value_all(PyObject *self, PyObject *args)
{

    value_handle vh, vh_id, vh_ch;
    PyObject *all;

  void * p_prop_handle;

  if (!PyArg_ParseTuple(args, "l", &p_prop_handle))
    return NULL;


    vh = ra_value_malloc();

    all = PyList_New(3);

    vh = ra_value_malloc();
    vh_id = ra_value_malloc();
    vh_ch = ra_value_malloc();
    if (ra_prop_get_value_all(p_prop_handle, vh_id, vh_ch, vh) != 0)
    {
      ra_value_free(vh);
      ra_value_free(vh_id);
      ra_value_free(vh_ch);

      return Py_None;
    }
    if (!ra_value_is_ok(vh) || !ra_value_is_ok(vh_id) || !ra_value_is_ok(vh_ch))
    {
      ra_value_free(vh);
      ra_value_free(vh_id);
      ra_value_free(vh_ch);

      return Py_None;
    }

    PyList_SetItem(all, 0, value_to_py_object(vh_id));
    PyList_SetItem(all, 1, value_to_py_object(vh_ch));
    PyList_SetItem(all, 2, value_to_py_object(vh));
 
    ra_value_free(vh);
    ra_value_free(vh_id);
    ra_value_free(vh_ch);

    return all;

} /* if_ra_prop_get_value_all() */

static PyObject *
if_ra_prop_get_events(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh, vh_min, vh_max;
    PyObject *ids;

  void * p_prop_handle;
  char * p_val_type;
  void * p_min;
  void * p_max;
  long p_ch;

  if (!PyArg_ParseTuple(args, "lslll", &p_prop_handle, &p_val_type, &p_min, &p_max, &p_ch))
    return NULL;


      vh_min = ra_value_malloc();
      vh_max = ra_value_malloc();

      ret = py_object_to_value((PyObject *)p_min, p_val_type, vh_min);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
	return Py_None;
      }
      ret = py_object_to_value((PyObject *)p_max, p_val_type, vh_max);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
	return Py_None;
      }

      vh = ra_value_malloc();
      ret = ra_prop_get_events(p_prop_handle, vh_min, vh_max, p_ch, vh);
      if (ret != 0)
      {
        ra_value_free(vh_min);
        ra_value_free(vh_max);
        ra_value_free(vh);
	return Py_None;
      }

    ids = value_to_py_object(vh);
 
    ra_value_free(vh);
    ra_value_free(vh_min);
    ra_value_free(vh_max);

    return ids;

} /* if_ra_prop_get_events() */

static PyObject *
if_ra_sum_add(PyObject *self, PyObject *args)
{

    sum_handle sum;
  
  void * p_class_handle;
  char * p_id;
  char * p_name;
  char * p_desc;
  long p_num_dim;
  void * p_dim_name;
  void * p_dim_unit;

  if (!PyArg_ParseTuple(args, "lssslll", &p_class_handle, &p_id, &p_name, &p_desc, &p_num_dim, &p_dim_name, &p_dim_unit))
    return NULL;


    sum = ra_sum_add(p_class_handle, p_id, p_name, p_desc, 
                     p_num_dim, p_dim_name, p_dim_unit);

    return Py_BuildValue("l", sum);
  } /* if_ra_sum_add() */

static PyObject *
if_ra_sum_add_ch(PyObject *self, PyObject *args)
{

    int ret;
  
  void * p_sum_handle;
  long p_ch;
  long p_fid_offset;

  if (!PyArg_ParseTuple(args, "lll", &p_sum_handle, &p_ch, &p_fid_offset))
    return NULL;


    ret = ra_sum_add_ch(p_sum_handle, p_ch, p_fid_offset);

    return Py_BuildValue("l", ret);
  } /* if_ra_sum_add_ch() */

static PyObject *
if_ra_sum_delete(PyObject *self, PyObject *args)
{

    long ok;
  
  void * p_sum_handle;

  if (!PyArg_ParseTuple(args, "l", &p_sum_handle))
    return NULL;


    ok = (long)ra_sum_delete(p_sum_handle);
    return Py_BuildValue("l", ok);
  } /* if_ra_sum_delete() */

static PyObject *
if_ra_sum_get(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh;
    PyObject *value;
  
  void * p_class_handle;
  char * p_id;

  if (!PyArg_ParseTuple(args, "ls", &p_class_handle, &p_id))
    return NULL;


    vh = ra_value_malloc();

    ret = ra_sum_get(p_class_handle, p_id, vh);
    if ((ret != 0) || !ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_sum_get() */

static PyObject *
if_ra_sum_add_part(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh;
    long part_id;
 
  void * p_sum_handle;
  void * p_events;

  if (!PyArg_ParseTuple(args, "ll", &p_sum_handle, &p_events))
    return NULL;


    vh = ra_value_malloc();
    if ((ret = py_object_to_value((PyObject *)p_events, "l", vh)) == 0)
      part_id = ra_sum_add_part(p_sum_handle, ra_value_get_num_elem(vh), ra_value_get_long_array(vh));
    else
      part_id = -1;

    ra_value_free(vh);

    return Py_BuildValue("l", part_id);
  } /* if_ra_sum_add_part() */

static PyObject *
if_ra_sum_del_part(PyObject *self, PyObject *args)
{

    int ret;
  
  void * p_sum_handle;
  long p_part_id;

  if (!PyArg_ParseTuple(args, "ll", &p_sum_handle, &p_part_id))
    return NULL;


    ret = ra_sum_del_part(p_sum_handle, p_part_id);
    return Py_BuildValue("l", ret);
  } /* if_ra_sum_del_part() */

static PyObject *
if_ra_sum_get_part_all(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh;
    PyObject *value;
  
  void * p_sum_handle;

  if (!PyArg_ParseTuple(args, "l", &p_sum_handle))
    return NULL;


    vh = ra_value_malloc();

    ret = ra_sum_get_part_all(p_sum_handle, vh);
    if ((ret != 0) || !ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;
  
  } /* if_ra_sum_get_part_all() */

static PyObject *
if_ra_sum_set_part_data(PyObject *self, PyObject *args)
{

  value_handle vh;
  int ret;

  void * p_sum_handle;
  long p_part_id;
  long p_ch_idx;
  long p_dim;
  char * p_data_type;
  void * p_data;

  if (!PyArg_ParseTuple(args, "llllsl", &p_sum_handle, &p_part_id, &p_ch_idx, &p_dim, &p_data_type, &p_data))
    return NULL;


  vh = ra_value_malloc();

  ret = py_object_to_value((PyObject *)p_data, p_data_type, vh);
  if (ret == 0)
    ra_sum_set_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim, vh);

  ra_value_free(vh);

  return Py_BuildValue("l", ret);

} /* if_ra_sum_set_part_data() */

static PyObject *
if_ra_sum_get_part_data(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh;
    PyObject *value;

  void * p_sum_handle;
  long p_part_id;
  long p_ch_idx;
  long p_dim;

  if (!PyArg_ParseTuple(args, "llll", &p_sum_handle, &p_part_id, &p_ch_idx, &p_dim))
    return NULL;


    vh = ra_value_malloc();

    ret = ra_sum_get_part_data(p_sum_handle, p_part_id, p_ch_idx, p_dim, vh);
    if ((ret != 0) || !ra_value_is_ok(vh))
    {
        ra_value_free(vh);
        return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;

} /* if_ra_sum_get_part_data() */

static PyObject *
if_ra_sum_get_part_events(PyObject *self, PyObject *args)
{

    int ret;
    value_handle vh;
    PyObject *value;

  void * p_sum_handle;
  long p_part_id;

  if (!PyArg_ParseTuple(args, "ll", &p_sum_handle, &p_part_id))
    return NULL;


    vh = ra_value_malloc();

    ret = ra_sum_get_part_events(p_sum_handle, p_part_id, vh);
    if ((ret != 0) || !ra_value_is_ok(vh))
    {
      ra_value_free(vh);
      return Py_None;
    }

    value = value_to_py_object(vh);

    ra_value_free(vh);

    return value;

} /* if_ra_sum_get_part_events() */

static PyObject *
if_ra_sum_set_part_events(PyObject *self, PyObject *args)
{

  value_handle vh;
  int ret;

  void * p_sum_handle;
  long p_part_id;
  PyObject * p_event_ids;

  if (!PyArg_ParseTuple(args, "llO", &p_sum_handle, &p_part_id, &p_event_ids))
    return NULL;


  vh = ra_value_malloc();

  ret = py_object_to_value(p_event_ids, "l", vh);
  if (ret == 0)
    ret = ra_sum_set_part_events(p_sum_handle, p_part_id, vh);

  ra_value_free(vh);

  return Py_BuildValue("l", (long)ret);

} /* if_ra_sum_set_part_events() */


static PyMethodDef RA_methods[] = {
  { "_if_lib_init", if_ra_lib_init, METH_VARARGS, "Initialize libRASCH and read all plugins. Must be called"
"    before any other function for libRASCH can be used."
"  "},
  { "_if_lib_close", if_ra_lib_close, METH_VARARGS, "Unload plugins and free memory."
"  "},
  { "_if_lib_get_error", if_ra_lib_get_error, METH_VARARGS, "Returns the last error occured inside of libRASCH."},
  { "_if_lib_handle_from_any_handle", if_ra_lib_handle_from_any_handle, METH_VARARGS, "Returns the ra-handle from any handle."},
  { "_if_lib_set_option", if_ra_lib_set_option, METH_VARARGS, ""
"Set an option."},
  { "_if_lib_get_option", if_ra_lib_get_option, METH_VARARGS, ""
"Get an option."},
  { "_if_lib_info_get", if_ra_lib_info_get, METH_VARARGS, "Returns the information 'info_name' about libRASCH. The"
"name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_plugin_info_get", if_ra_plugin_info_get, METH_VARARGS, "Returns the information 'info_name' about the plugin"
"'plugin_handle'. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_result_info_get", if_ra_result_info_get, METH_VARARGS, "Returns the information 'info_name' about the result"
"number 'result_num' from plugin"
"'plugin_handle'. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_meas_info_get", if_ra_meas_info_get, METH_VARARGS, "Returns the information 'info_name' about a"
"measurement. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_obj_info_get", if_ra_obj_info_get, METH_VARARGS, "Returns the information 'info_name' about a"
"measurement-object. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_rec_info_get", if_ra_rec_info_get, METH_VARARGS, "Returns the information 'info_name' about a"
"recording. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_dev_info_get", if_ra_dev_info_get, METH_VARARGS, "Returns the information 'info_name' about a"
"measurement-device. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_ch_info_get", if_ra_ch_info_get, METH_VARARGS, "Returns the information 'info_name' about a"
"recorded channel. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_eval_info_get", if_ra_eval_info_get, METH_VARARGS, "Returns the information 'info_name' about an evaluation."
"The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_class_info_get", if_ra_class_info_get, METH_VARARGS, "Returns the information 'info_name' about an"
"evaluation-class. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_prop_info_get", if_ra_prop_info_get, METH_VARARGS, "Returns the information 'info_name' about an"
"event-property. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_summary_info_get", if_ra_summary_info_get, METH_VARARGS, "Returns the information 'info_name' about an"
"event-summary. The name and a short description is also returned. See"
"XXX for available informations."},
  { "_if_sum_dim_info_get", if_ra_sum_dim_info_get, METH_VARARGS, "Returns the information 'info_name' about the dimension"
"'dim' of the event-summary. The name and a short description is also"
"returned. See XXX for available informations."},
  { "_if_sum_ch_info_get", if_ra_sum_ch_info_get, METH_VARARGS, "Returns the information 'info_name' about the channel"
"'channel' of the event-summary. The name and a short description is"
"also returned. See XXX for available informations."},
  { "_if_info_get_by_idx", if_ra_info_get_by_idx, METH_VARARGS, "Returns the information number 'index' from the info-type"
"'info_type'. The name and a short description is also returned. See"
"XXX for available informations and for valid 'info_type'."},
  { "_if_info_set", if_ra_info_set, METH_VARARGS, "Set the information 'info_name' to 'info_value' for the object"
"handled by 'handle'."},
  { "_if_meas_handle_from_any_handle", if_ra_meas_handle_from_any_handle, METH_VARARGS, "Returns a measurement-handle from any handle."},
  { "_if_meas_find", if_ra_meas_find, METH_VARARGS, "Returns all signales handled by libRASCH in the directory 'directory'."},
  { "_if_meas_open", if_ra_meas_open, METH_VARARGS, "Opens the measurement 'signal_name'. If you are"
"interested only in information about the type of the measurement or"
"about informations of the measurement-object than the flag 'open_fast'"
"can be set to '1' and the opening is faster (the reading of the"
"evaluations are skipped.) A specific evaluation file can be given"
"in 'eval_file'. If this is set to NULL or the empty string, libRASCH"
"will look for the default evaluation file associated with the"
"measurement."},
  { "_if_meas_new", if_ra_meas_new, METH_VARARGS, "Creates a new measurement with the name @name in the"
"directory @dir. The function returns the handle to the"
"measurement. The measurement will be stored using the libRASCH"
"file-format."},
  { "_if_meas_save", if_ra_meas_save, METH_VARARGS, "Saves the changes to a measurement (NOT to the"
"evaluation). Plugin must support this."},
  { "_if_meas_close", if_ra_meas_close, METH_VARARGS, "Close the measurement with the measurement-handle 'meas_handle'."},
  { "_if_rec_get_first", if_ra_rec_get_first, METH_VARARGS, "Returns the first recording-handle for session number"
"'session_number'."},
  { "_if_rec_get_next", if_ra_rec_get_next, METH_VARARGS, "Returns the recording-handle after 'rec_handle'."},
  { "_if_rec_get_first_child", if_ra_rec_get_first_child, METH_VARARGS, "Returns the recording-handle which is the first child of"
"'rec_handle'. "},
  { "_if_raw_get", if_ra_raw_get, METH_VARARGS, "Returns 'num_samples' raw-data starting with sample"
"'start_sample' from recording 'rec_handle' and channel 'ch'."},
  { "_if_raw_get_unit", if_ra_raw_get_unit, METH_VARARGS, "Returns 'num_samples' raw-data starting with sample"
"'start_sample' from recording 'rec_handle' and channel 'ch'. The values"
"returned are scaled so the values are 'unit'-values"},
  { "_if_meas_copy", if_ra_meas_copy, METH_VARARGS, ""
"The function copy the files of measurement meas_handle to directory dir."
""
"IMPORTANT!!!"
"Function _not_ completely tested. Use at your own risk."},
  { "_if_meas_move", if_ra_meas_move, METH_VARARGS, ""
"The function move the files of the measurement to directory dir. After the move"
"the measurement will be reopend (to be able to handle the new positions of the files)"
"and the functions returns the new measurement-handle."
""
"IMPORTANT!!!"
"Function _not_ completely tested. Use at your own risk."},
  { "_if_meas_delete", if_ra_meas_delete, METH_VARARGS, ""
"The function deletes the files of the measurement. The measurement"
"will be closed after the deletion. Therefore meas_handle is no longer a valid"
"measurement-handle."
""
"IMPORTANT!!!"
"Function _not_ completely tested. Use at your own risk."},
  { "_if_lib_use_plugin", if_ra_lib_use_plugin, METH_VARARGS, "Not implemented yet."},
  { "_if_plugin_get_by_num", if_ra_plugin_get_by_num, METH_VARARGS, "Returns the plugin-handle for plugin number 'number'."},
  { "_if_plugin_get_by_name", if_ra_plugin_get_by_name, METH_VARARGS, "Returns the plugin-handle for plugin 'name'."},
  { "_if_proc_get", if_ra_proc_get, METH_VARARGS, ""},
  { "_if_proc_do", if_ra_proc_do, METH_VARARGS, "Perform some calculation using the processing-plugin"
"'plugin_handle. The function returns a process-handle, which will be"
"used in the function 'ra_result_get' to get the results of the"
"calculation."},
  { "_if_proc_get_result_idx", if_ra_proc_get_result_idx, METH_VARARGS, "The function returns the index of the result"
"'res_ascii_id' in the results returned in re_proc_get_result()."},
  { "_if_proc_get_result", if_ra_proc_get_result, METH_VARARGS, "Returns the results of the calculation done calling"
"'ra_proc_do'. It also returns the name's and short descriptions"
"of the values. The returning variables are vectors containing all"
"results."},
  { "_if_proc_free", if_ra_proc_free, METH_VARARGS, "The function ra_proc_do() allocates some memory to"
"store the results. This function frees the allocated memory."},
  { "_if_eval_save", if_ra_eval_save, METH_VARARGS, "Save evaluation infos of a measurement."
"  "},
  { "_if_eval_attribute_list", if_ra_eval_attribute_list, METH_VARARGS, ""
"    Get the list of available attributes for evaluation related handle 'handle'."
"  "},
  { "_if_eval_attribute_get", if_ra_eval_attribute_get, METH_VARARGS, ""
"    Get the value of the attribute 'attrib_name'."
"  "},
  { "_if_eval_attribute_set", if_ra_eval_attribute_set, METH_VARARGS, ""
"    Set the value of the attribute 'attrib_name'."
"  "},
  { "_if_eval_attribute_unset", if_ra_eval_attribute_unset, METH_VARARGS, ""
"    Unset the value of the attribute 'attrib_name'."
"  "},
  { "_if_eval_add", if_ra_eval_add, METH_VARARGS, "Returns the evaluation-handle of the newly added evaluation."
"  "},
  { "_if_eval_delete", if_ra_eval_delete, METH_VARARGS, "Delete the evaluation 'eval_handle'."
"  "},
  { "_if_eval_get_all", if_ra_eval_get_all, METH_VARARGS, ""
"    Returns all evaluations belonging to 'meas_handle'."
"  "},
  { "_if_eval_get_original", if_ra_eval_get_original, METH_VARARGS, "Returns the evaluation-handle of the original evaluation."
"  "},
  { "_if_eval_get_default", if_ra_eval_get_default, METH_VARARGS, "Returns the evaluation-handle of the default evaluation."
"  "},
  { "_if_eval_set_default", if_ra_eval_set_default, METH_VARARGS, "Set the evaluation 'eval_handle' as the default one."
"  "},
  { "_if_eval_get_handle", if_ra_eval_get_handle, METH_VARARGS, "Returns the evaluation-handle the event-class 'class_handle' belongs to."
"  "},
  { "_if_class_add", if_ra_class_add, METH_VARARGS, "Returns the handle of the newly added event-class."
"  "},
  { "_if_class_add_predef", if_ra_class_add_predef, METH_VARARGS, "Returns the handle of the newly added event-class."
"  "},
  { "_if_class_delete", if_ra_class_delete, METH_VARARGS, "Delete the event-class given by 'class_handle'."
"  "},
  { "_if_class_get", if_ra_class_get, METH_VARARGS, ""
"    Returns all event-classes with the id 'id'."
"  "},
  { "_if_class_add_event", if_ra_class_add_event, METH_VARARGS, "Returns the id of the added event."
"  "},
  { "_if_class_del_event", if_ra_class_del_event, METH_VARARGS, "Delete the event with the id 'event_id'."
"  "},
  { "_if_class_get_event_pos", if_ra_class_get_event_pos, METH_VARARGS, "Returns the start- and end-position of the event 'event_id'."
"  "},
  { "_if_class_set_event_pos", if_ra_class_set_event_pos, METH_VARARGS, "Set the start- and end-position of the event 'event_id'."
"  "},
  { "_if_class_get_events", if_ra_class_get_events, METH_VARARGS, ""
"    Returns all events which are between 'start_pos' and 'end_pos'. If the flag"
"    'complete' is set, only these events are returned which starts AND ends in"
"    region. If the flag 'sort' is set, the returned events are sorted according"
"    to their position."
"  "},
  { "_if_class_get_prev_event", if_ra_class_get_prev_event, METH_VARARGS, "Returns the event-id of the previous event."
"  "},
  { "_if_class_get_next_event", if_ra_class_get_next_event, METH_VARARGS, "Returns the event-id of the next event."
"  "},
  { "_if_class_get_handle", if_ra_class_get_handle, METH_VARARGS, "Returns the class-handle the property or summary handle 'handle' belongs to."
"  "},
  { "_if_prop_add", if_ra_prop_add, METH_VARARGS, "Returns the handle of the added event-property."
"  "},
  { "_if_prop_add_predef", if_ra_prop_add_predef, METH_VARARGS, "Returns the handle of the added event-property."
"  "},
  { "_if_prop_delete", if_ra_prop_delete, METH_VARARGS, "Delete the event-property given by 'prop_handle'."
"  "},
  { "_if_prop_get", if_ra_prop_get, METH_VARARGS, ""
"    Return the event-propertiy with the id 'id'."
"  "},
  { "_if_prop_get_all", if_ra_prop_get_all, METH_VARARGS, ""
"    Returns all event-properties within event-class 'class_handle'."
"  "},
  { "_if_prop_set_value", if_ra_prop_set_value, METH_VARARGS, ""
"Set an event-property."},
  { "_if_prop_set_value_mass", if_ra_prop_set_value_mass, METH_VARARGS, ""
"Set all values of an event-property."},
  { "_if_prop_get_ch", if_ra_prop_get_ch, METH_VARARGS, "Returns for event 'event-id' a list of channels for which"
"values are available."},
  { "_if_prop_get_value", if_ra_prop_get_value, METH_VARARGS, "Returns the event-property value for event 'event-id' and channel 'channel'."},
  { "_if_prop_get_value_all", if_ra_prop_get_value_all, METH_VARARGS, "Returns all event-property values and the corresponding event-ids channels."},
  { "_if_prop_get_events", if_ra_prop_get_events, METH_VARARGS, "Return the event-ids which values are between 'min' and 'max."},
  { "_if_sum_add", if_ra_sum_add, METH_VARARGS, "Returns the handle of the added event-summary."
"  "},
  { "_if_sum_add_ch", if_ra_sum_add_ch, METH_VARARGS, "Add a channel to the event-summary 'sum_handle'."
"  "},
  { "_if_sum_delete", if_ra_sum_delete, METH_VARARGS, "Delete the event-summary given by 'sum_handle'."
"  "},
  { "_if_sum_get", if_ra_sum_get, METH_VARARGS, ""
"    Returns all event-summaries with the id 'id'."
"  "},
  { "_if_sum_add_part", if_ra_sum_add_part, METH_VARARGS, "Add a summary part; the part-id is returned.  "
"  "},
  { "_if_sum_del_part", if_ra_sum_del_part, METH_VARARGS, "Delete a summary part."
"  "},
  { "_if_sum_get_part_all", if_ra_sum_get_part_all, METH_VARARGS, ""
"    Returns all parts in the event-summary 'sum_handle'."
"  "},
  { "_if_sum_set_part_data", if_ra_sum_set_part_data, METH_VARARGS, ""
"Set the data for an event-summary."},
  { "_if_sum_get_part_data", if_ra_sum_get_part_data, METH_VARARGS, "Returns the data of an event-summary part."},
  { "_if_sum_get_part_events", if_ra_sum_get_part_events, METH_VARARGS, "Returns for an event-summary part the event-ids which are the part is based on."},
  { "_if_sum_set_part_events", if_ra_sum_set_part_events, METH_VARARGS, "Returns for an event-summary part the event-ids which are the part is based on."},
  { NULL, NULL, 0, NULL}
};

void
initRASCH_if(void)
{
	PyObject *m, *d, *desc;
	m = Py_InitModule("RASCH_if", RA_methods);
	d = PyModule_GetDict(m);
	desc = PyString_FromString("Interface to access libRASCH from Python.");
	PyDict_SetItemString(d, "__doc__", desc);
}  /* initRASCH_if() */
