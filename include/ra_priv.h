/**
 * \file ra_priv.h
 *
 * The header file contains definitions for private use in the library
 * core and in the plugins.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _RA_PRIV_H
#define _RA_PRIV_H

#include <libintl.h>
#include <ra_defines.h>
#include <ra_eval.h>
#include <ra_pl_comm.h>


#ifdef __cplusplus
extern "C" {
#endif

/* do gettext defines first */
#define _(String) gettext (String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

/*
#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
  #define bindtextdomain(Package, Directory)*/



/* ------------------------------ globals ------------------------------ */
/* This variable (set in libRASCH init-function()) controls if 
   debug-infos will be send to stderr. The variable will be set to 1
   if the environment variable "LIBRASCH_DEBUG" exists. */
extern int ra_print_debug_infos;

/* variables for i18n */
#define MAX_I18N_LEN  250
extern char ra_i18n_codeset[MAX_I18N_LEN];


/* ------------------------------ defines ------------------------------ */
/* standard functions in plugin which _must_ be available in plugin */
#define LOAD_PLUGIN_FUNC  "load_ra_plugin"
#define INIT_PLUGIN_FUNC  "init_ra_plugin"
#define FINI_PLUGIN_FUNC  "fini_ra_plugin"

/* license types for plugins */
#define LICENSE_GPL            0
#define LICENSE_LGPL           1
#define LICENSE_PROPRIETARY    2
#define LICENSE_NOT_CHOOSEN    3

#define _ra_set_error(h, nr, str)  _ra_set_error_int(h, nr, str, __FILE__, __LINE__)


/* ------------------------------ struct's ------------------------------ */
/* forward declarations */
struct librasch;
struct ra_meas;

/**
 * \struct ra_rec
 * \brief contain infos about a recording
 * \param <handle_id> set to RA_HANDLE_REC
 * \param <next> pointer to next recording
 * \param <prev> pointer to previous recording
 * \param <child> pointer to sub-recording
 * \param <parent> pointer to parent recording
 * \param <meas> pointer to the measurement
 * \param <private_data> some private data (will be no longer used in the near future)
 */
struct ra_rec
{
	unsigned short handle_id;

	struct ra_rec *next;
	struct ra_rec *prev;
	struct ra_rec *child;
	struct ra_rec *parent;

	struct ra_meas *meas;

	void *private_data;	/* can be used to store rec. specific info */
}; /* struct ra_rec */


/**
 * \struct ra_session
 * \brief infos about a recording session
 * \param <name> name of the session
 * \param <desc> description of the session
 * \param <root_rec> pointer to the root recording of this session
 */
struct ra_session
{
	char name[RA_VALUE_NAME_MAX];
	char desc[RA_VALUE_DESC_MAX];

	struct ra_rec *root_rec;
}; /* struct ra_session */


/**
 * \struct find_meas
 * \brief structure handle infos when measurement-search functions are used
 * \param <handle_id> set to RA_HANDLE_FIND
 * \param <num> number of the found measurements
 * \param <curr> number of last returned file-name
 * \param <names> array of strings with the found file-names
 * \param <plugins> array of strings with the plugin-names handling the files
 */
struct find_meas
{
	unsigned short handle_id;

	int num;
	int curr;

	char **names;
	char **plugins;
}; /* struct find_meas */


/**
 * \struct plugin_infos
 * \brief  contains infos about a plugin
 * \param <name> name of plugin
 * \param <desc> description of plugin
 * \param <file> file name (incl. path) of the loaded plugin
 * \param <use_plugin> flag if plugin should be used
 * \param <type> bit-mask showing the type of the plugin (see PLUGIN_* in ra_defines.h)
 * \param <version> version string of the plugin
 * \param <build_ts> time-stamp when pluign was build
 * \param <license> license of plugin (see LICENSE_* above)
 * \param <num_options> number of options in the plugin
 * \param <opt> infos about the options in the plugin
 * \param <num_results> number of results the plugin provide
 * \param <res> infos about the results in the plugin
 * \param <num_create_class> number of event-classes
 * \param <create_class> infos about the event-classes
 * \param <num_create_prop> number of event-properties
 * \param <create_prop> infos about the event-properties
 */
struct plugin_infos
{
	char name[RA_VALUE_NAME_MAX];
	char desc[RA_VALUE_DESC_MAX];
	char file[MAX_PATH_RA];

	int use_plugin;

	unsigned long type;

	char version[10];
	char build_ts[50];
	int license;

	long num_options;
	struct ra_option_infos *opt;

	long num_results;
	struct ra_result_infos *res;

	long num_create_class;
	struct ra_auto_create_class *create_class;

	long num_create_prop;
	struct ra_auto_create_prop *create_prop;
}; /* struct plugin_infos */


/**
 * \struct plugin_access_func
 * \brief contains pointers to access-functions in plugin, will be set IN plugin
 * \param <check_meas> checks if given file-name is a measurement which can be handled by the plugin
 * \param <find_meas> looks in 'dir' for measurements which can be handled by the plugin
 * \param <open_meas> open measurement
 * \param <new_meas> creates new measurement (not implemented yet)
 * \param <save_meas> save measurement (not implemented yet)
 * \param <close_meas> close opened files and free allocated memory
 * \param <get_info_id> returns informations selected by id-number
 * \param <get_info_idx> returns informations selected by index-number
 * \param <set_info> set information in a measurement
 * \param <rec_add> add a recording (not implemented yet)
 * \param <dev_add> add a recroding-device (not implemented yet)
 * \param <ch_add> add a recording-channel (not implemented yet)
 * \param <session_new> starts a new recording session (not implemented yet)
 * \param <add_raw> add raw signal data (not implemented yet)
 * \param <get_raw> returns unscaled raw signal data as DWORD values
 * \param <get_raw_unit> returns raw signal data scaled to 'unit'-values
 * \param <get_eval_info> returns infos about an evaluation
 * \param <get_evprpp_num> returns number of available event properties
 * \param <get_evprop_info> returns infos about an event property
 * \param <get_ev_num> returns number of events for a event property
 * \param <get_ev_start_end> returns start and end of an event
 * \param <get_ev_value> returns event value
 * \param <get_ev_value_inf> returns event value in a value-handle
 * \param <get_ev_value_all> returns event values for all events
 * \param <get_ev_start_all> returns start of events for all events
 * \param <get_ev_end_all> returns end of events for all events
 * \param <get_template_info> returns infos about a template
 * \param <get_template_raw> returns template-data
 */
struct plugin_access_func
{
	/* check if measurement name is handled by this plugin */
	int (*check_meas) (const char *name);

	int (*find_meas) (const char *dir, struct find_meas * f);

	meas_handle (*open_meas) (ra_handle ra, const char *name, int fast);
	meas_handle (*new_meas) (const char *dir, const char *name);
	int (*save_meas) (meas_handle mh);
	int (*close_meas) (meas_handle mh);

	int (*get_info_id) (any_handle h, int id, value_handle vh);
	int (*get_info_idx) (any_handle h, int info_type, int idx, value_handle vh);

	int (*set_info) (any_handle h, int id, value_handle vh);

	rec_handle (*rec_add)(meas_handle mh, rec_handle parent);
	int (*dev_add)(rec_handle rh);
	int (*ch_add)(rec_handle rh);

	int (*session_new)(meas_handle mh);

	size_t(*add_raw)(meas_handle mh, int channel, value_handle vh);

	size_t(*get_raw) (meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, DWORD *data, DWORD *data_high);
	size_t(*get_raw_unit) (meas_handle mh, rec_handle rh, int ch, size_t start, size_t num_data, double *data);

	int (*get_eval_info)(meas_handle mh, value_handle vh_name, value_handle vh_desc, value_handle add_ts,
			     value_handle modify_ts, value_handle user, value_handle host, value_handle prog);
	long (*get_class_num)(meas_handle mh);
	int (*get_class_info)(meas_handle mh, unsigned long class_num, value_handle vh_id, value_handle vh_name, value_handle vh_desc);
	long (*get_prop_num)(class_handle clh, unsigned long class_num);
	int (*get_prop_info)(class_handle clh, unsigned long class_num, unsigned long prop_num, value_handle vh_id, value_handle vh_type,
			     value_handle vh_len, value_handle vh_name, value_handle vh_desc, value_handle vh_unit,
			     value_handle vh_use_minmax, value_handle vh_min, value_handle vh_max,
			     value_handle vh_has_ign_value, value_handle vh_ign_value);
	int (*get_ev_info)(class_handle clh, unsigned long class_num, value_handle vh_start, value_handle vh_end);
	int (*get_ev_value)(prop_handle ph, unsigned long class_num, unsigned long prop_num, long event, value_handle vh_value, value_handle vh_ch);
	int (*get_ev_value_all)(prop_handle ph, unsigned long class_num, unsigned long prop_num, value_handle vh_value, value_handle vh_ch);

	long (*get_sum_num)(class_handle clh, unsigned long class_num);
	int (*get_sum_info)(class_handle clh, unsigned long class_num, unsigned long sum_num, value_handle vh_id, 
			    value_handle vh_name, value_handle vh_desc,
			    value_handle vh_dim_name, value_handle vh_dim_unit,
			    value_handle vh_ch, value_handle vh_fid_point, unsigned long *num_part);
	int (*get_sum_events)(sum_handle sh, unsigned long class_num, unsigned long sum_num, unsigned long part_num, value_handle vh_events);
	int (*get_sum_part_data)(sum_handle sh, unsigned long class_num, unsigned long sum_num, unsigned long part_num,
				 unsigned long ch, unsigned long dim, value_handle vh);
}; /* struct plugin_access_func */


/**
 * \struct plugin_process_func
 * \brief structure with pointers to processing functions in plugin, will be set IN plugin
 * \param <get_info_id> returns infos selected by id
 * \param <get_proc_handle> returns an initialized processing handle
 * \param <free_proc_handle> frees processing handle and allocated memory for the proc-handle
 * \param <do_processing> performs the processing algorithm of the plugin
 */
struct plugin_process_func
{
	int (*get_info_id) (int id, value_handle vh);

	proc_handle (*get_proc_handle)(plugin_handle pl);
	void (*free_proc_handle)(proc_handle proc);

	int (*do_processing)(proc_handle proc);
}; /* struct plugin_process_func */


/**
 * \struct plugin_view_func
 * \brief structure with pointers to viewing functions in plugin, will be set IN plugin
 * \param <check_channels> checks if the type of a recording channel can be shown using this plugin
 * \param <check_plugin_usage> checks if the given plugin is used by this plugin
 * \param <get_view> returns widget-/window-handle of the view handled by the plugin
 */
struct plugin_view_func
{
	int (*check_channels) (rec_handle rh, long ch);
	int (*check_plugin_usage) (const char *pl_name);

	view_handle (*get_view_handle)(plugin_handle pl);
	void (*free_view_handle)(view_handle vih);
	int (*create_view) (view_handle vih);
}; /* struct plugin_view_func */


/**
 * \struct plugin_struct
 * \brief contains all necessary info about a plugin
 * \param <handle_id> set to RA_HANDLE_PLUGIN
 * \param <next> pointer to next plugin
 * \param <prev> pointer to previous plugin
 * \param <dl> handle to loaded dynamic library
 * \param <ra> handle to the libRASCH-instance loaded the plugin
 * \param <info> structure of infos about the plugin
 * \param <access> structure which contains function pointers needed by access plugins
 * \param <proc> structure which contains function pointers needed by process plugins
 * \param <view> structure which contains function pointers needed by view plugins
 * \param <call_gui> pointer to a function showing a GUI element (e.g. dialogs)
 * \param <handle_signal> pointer to a function handling signals
 * \param <set_option> no longer used
 * \param <get_option> no longer used
 */
struct plugin_struct
{
	unsigned short handle_id;

	struct plugin_struct *next;
	struct plugin_struct *prev;
	void *dl;

	ra_handle ra;

	struct plugin_infos info;	/* definition in ra_defines.h; will be set _in_ plugin;
					   will be used for get plugin infos */

	struct plugin_access_func access;
	struct plugin_process_func proc;
	struct plugin_view_func view;

	/* pointer to function calling gui (e.g. dialogs); this must be available for all type
	   of plugins */
	int (*call_gui) (any_handle h);

	/* for inter-plugin communication */
	void (*handle_signal) (meas_handle mh, dest_handle dh, const char *sig_name, int num_para, struct comm_para * p);

	/* for setting/getting options (no longer used, but perhaps when options used by other plugins,
	   the functions can be useful; therefore they will stay here) */
	int (*set_option) (proc_handle proc, char *opt_name, value_handle vh);
	int (*get_option) (proc_handle proc, char *opt_name, value_handle vh);
}; /* struct plugin_struct */


/**
 * \struct plugin_helpers
 * \brief I do not know what it is this for
 * \param <get_plugin> ???
 */
struct plugin_helpers
{
	struct plugin_struct *(*get_plugin)(struct librasch *ra, const char *name);
}; /* struct plugin_helpers */


/**
 * \struct config_info
 * \brief infos set in config-file
 * \param <plugin_dir> directory where the plugins are loaded from
 * \param <po_dir> directory where the translation-files are stored
 * \param <num_ch_map_system> number of ch-map entries in system ch-map
 * \param <ch_map_system> system-wide ch-map (set in config file/registry entries)
 */
struct config_info
{
	char plugin_dir[MAX_PATH_RA];
	char po_dir[MAX_PATH_RA];

	int num_ch_map_system;
	struct ch_map *ch_map_system;
}; /* struct config_info */


/**
 * \struct librasch
 * \brief contains informations about a libRASCH instance
 * \param <handle_id> set to RA_HANDLE_LIB
 * \param <config> structure with configuration infos
 * \param <pl_head> head of linked list containing the plugin-struct's
 * \param <hf> helper functions (???)
 * \param <error_number> last error number
 * \param <error_from> string describing where the error comes from (optional)
 * \param <err_number_lib> error-number from an external entity (if error comes from an entity) (not used yet)
 * \param <err_desc_lib> a description from an external entity (if error comes from an entity) (not used yet)
 */
struct librasch
{
	unsigned short handle_id;

	struct config_info config;
	struct plugin_struct *pl_head;	/* linked list to all plugins */
	struct plugin_helpers hf;	/* structure with helper functions */

	/* error number:
	   - negative values: os/c-lib specific errors
	   - postive values:  libRASCH specific errors */
	long error_number;
	/* a short desc where the error comes from (e.g. libRASCH, plugin-oxford, plugin-hrt)
	   and the name of the source-file (incl. line number) where the error was set */
	char *error_from;
	long err_number_lib;
	char *err_desc_lib;
}; /* struct librasch */


/**
 * \struct eval_info
 * \brief contains informations about all libRASCH evaluations associated with a measurement
 * \param <filename> name of the evaluation-file given by the user (can be "")
 * \param <attribute> linked list of all attributes
 * \param <evaluation> linked list of all evaluations
 */
struct eval_info
{
	char filename[MAX_PATH_RA];

	struct eval_attribute *attribute;

	struct eval *evaluations;
}; /* struct eval_info */


/* measurement specific structure
   contains pointers to
   - plugin which handles measurement,
   - plugin-private-infos
   - array with pointers to sessions (which include pointers to recordings)
   - evaluation(s) of measurement
   - communication between plugins
   - scaling-factors for x-axis
*/
/**
 * \struct ra_meas
 * \brief contains measurement infos
 * \param <handle_id> set to RA_HANDLE_MEAS
 * \param <ra> pointer to general libRASCH structure
 * \param <p> pointer to the plugin-struct handling the measurement
 * \param <priv> plugin-specific infos about the measurement
 * \param <num_sessions> number of recording sessions in the measurement
 * \param <sessions> array of recording sessions
 * \param <eval> structure with the evaluation infos
 * \param <plc> structure containing the signal-infos for the measurement
 * \param <num_ch> number of recording channels in the measurement
 * \param <x_scale> array of maximum-samplerate-scaling-factors for each channel
 * \param <max_samplerate> maximum samplerate used in the recording
 */
struct ra_meas
{
	unsigned short handle_id;

	struct librasch *ra;

	struct plugin_struct *p;	/* plugin which handles measurement */

	/* plugin-specific infos about this measurement */
	void *priv;

	int num_sessions;
	struct ra_session *sessions;

	struct eval_info eval;

	struct plugin_comm plc;

	/* scaling factors for the x-axis for every single channel;
	   needed for calculating the position if measurement has
	   different sampling rates */
	long num_ch;  /* for parameter checking in func ra_get_ch_x_scale() */
	double *x_scale;
	double max_samplerate;
}; /* struct ra_meas */


/* Plugins which have options and want to support the getting/setting of options
   with function calls, can use this structure to store the information. */
/**
 * \struct ra_option_infos
 * \brief Plugins which have options and want to support the getting/setting of options with function calls, can use this structure to store the information.
 * \param <name> name of the option
 * \param <desc> description of the option
 * \param <type> value-type of the option
 * \param <offset> offset (in bytes) of the option in the option structure
 * \param <num_offset> offset (in bytes) of the number of elements if the option is an array of values
 */
struct ra_option_infos
{
	const char *name;
	const char *desc;
	long type;
	long offset;
	/* The next element is only needed when the option-type is an array.
	   The variable described will hold the number of elements in the array.
	   It must be from type long. */
	long num_offset; 	/* offset of variable where #elements can be found */
}; /* struct ra_option_infos */


/**
 * \struct ra_result_infos
 * \brief Plugins "creating" results can use this structure to store information about the results.
 * \param <name> a short name of the result
 * \param <desc> a description of the result
 * \param <type> type of the result
 * \param <def> flag if the result belongs to the default results
 */
struct ra_result_infos
{
	const char *name;
	const char *desc;
	int type;
	int def;
}; /* struct ra_result_infos */


/**
 * \struct proc_info
 * \brief infos about processing which will be used in plugins
 * \param <handle_id> id to indentify type of structure
 * \param <plugin> process-plugin this struct "belongs" to
 * \param <ra> librasch handle
 * \param <mh> measurement handle
 * \param <rh> record handle
 * \param <callback> function pointer to show progress
 * \param <num_options> number of options
 * \param <options> pointer to plugin-specific option-struct
 * \param <num_result_sets> number of available result-sets (when multiple event-classes were processed)
 * \param <num_results> number of the available results
 * \param <results> array of value_handle's holding the results ('num_result_sets' * 'num_results' elements)
 */
struct proc_info
{
	unsigned short handle_id;

	plugin_handle plugin;

	ra_handle ra;
	meas_handle mh;
	rec_handle rh;

	void (*callback) (const char *text, int percent);

	long num_options;
	void *options;

	long num_result_sets;
	long num_results;
	value_handle *results;
};  /* struct proc_info */


/**
 * \struct view_info
 * \brief infos about views, will be used in view-plugins
 * \param <handle_id> id to indentify type of structure
 * \param <ra> librasch handle
 * \param <mh> measurement handle
 * \param <rh> record handle
 * \param <eh> evaluation handle
 * \param <plugin> process-plugin this struct "belongs" to
 * \param <num_ch> number of channels which should be shown in view (= -1: all channels)
 * \param <ch> can be NULL if num_ch == -1
 * \param <parent> parent widget (will be cast in the plugin to the right type; depends on GUI-Framework)
 * \param <name> name of the view (FIXME: for what is the name used?)
 * \param <signal> signal object in GUI
 * \param <pos_name> name of position event-property
 * \param <templ_name> name of event-property which holds template-numbers (only needed for template-views)
 * \param <num_views> number of views handled by the plugin
 * \param <views> handle(s) of created view(s)
 * \param <real_views> above handle can be a frame of the view, this is the handle of the view
 * \param <num_options> number of options
 * \param <options> pointer to plugin-specific option-struct
 */
struct view_info
{
	unsigned short handle_id;

	plugin_handle plugin;

	ra_handle ra;
	meas_handle mh;
	rec_handle rh;

	/* list of channels which should be shown in view */
	int num_ch;		/* -1: all views */
	int *ch;		/* can be NULL if num_ch == -1 */

	void *parent;

	char name[200];		/* FIXME: use const */
	void *signal;		/* signal object in GUI (FIXME: better explanation, after remembering what it means) */

	char pos_name[64];	/* name of position event-property */
	char templ_name[64];	/* name of event-property which holds template-numbers (only needed for template-views) */

	/* handle(s) of created view(s), it can be more than one view if ra_view-module
	   is used and the the recording has sub-recordings (TODO: find better way to organize returning of views) */
	int num_views;
	void **views;
	void **real_views;

	/*  At the moment there is no real option-handling for views. The variable options is just used
	    to transfer an address and the source using this has to know what is stored there.
	    TODO: Think about options for view-plugins (there are some var's in this structure which are
	    options and should be set as the options in the proc-plugins). */
	long num_options;
	void *options;
};  /* struct view_info */


/* ---------- prototypes ---------- */
/* "external" interface but should only be used from libRASCH or plugin code */
LIBRAAPI void _ra_set_error_int(any_handle h, long error, const char *error_from, const char *src_file, int src_line);

LIBRAAPI int ra_eval_save_result(eval_handle eh, class_handle clh, proc_handle proc, long res_set);

void get_error_text(long err, char *text, size_t len); /* os specific function; get os/c-lib error
                                                          or calls get_error_text_ra() */
void get_error_text_ra(long err, char *text, size_t len);

void set_endian_type();
LIBRAAPI int get_endian_type();

void init_i18n(struct librasch *ra);
LIBRAAPI int local_to_utf8(const char *src, char *dest, size_t dest_size);
LIBRAAPI int local_to_utf8_inplace(char *string, size_t buf_size);
LIBRAAPI int to_utf8(const char *src, char *dest, size_t dest_size, const char *src_encoding);
LIBRAAPI int to_utf8_inplace(char *string, size_t buf_size, const char *src_encoding);
LIBRAAPI int utf8_to_local(const char *src, char *dest, size_t dest_size);
LIBRAAPI int utf8_to_local_inplace(char *string, size_t buf_size);
LIBRAAPI int from_utf8(const char *src, char *dest, size_t dest_size, const char *dest_encoding);
LIBRAAPI int from_utf8_inplace(char *string, size_t buf_size, const char *dest_encoding);

int read_config(struct librasch *ra);
void init_config(struct config_info *conf);
#ifdef WIN32
int read_win32_registry(struct librasch *ra);
#endif
int find_config_file(char *fn);
int read_modules(struct librasch *ra);
int dload_module(struct librasch *ra, char *file);
void close_modules(struct librasch *ra);
struct plugin_struct *get_plugin(struct librasch *ra, int index);
struct plugin_struct *get_plugin_by_name(struct librasch *r, const char *name);
int calc_x_scales(struct ra_meas *meas);

int check_handle_type(any_handle h, int id);
int get_lib_info(ra_handle ra, int id, value_handle vh);
int get_plugin_info(plugin_handle pl, int id, value_handle vh);
int get_meas_general_info(meas_handle meas, int id, value_handle vh);
int get_session_info(any_handle h, int id, value_handle vh);
int get_proc_info(any_handle h, int id, value_handle vh);

struct plugin_struct *get_plugin_from_handle(any_handle h);

int find_option(const char *name, struct ra_option_infos *infos, int num_opt);
int get_option(void *options, struct ra_option_infos *opt_info, value_handle vh, long num_offset);
int set_option(void *options, struct ra_option_infos *opt_info, value_handle vh, long num_offset);

int valid_value_handle(value_handle vh, int line_num);
LIBRAAPI int set_meta_info(value_handle vh, const char *name, const char *desc, int id);

int copy_move_meas(meas_handle mh, const char *dest_dir, int copy);


/* ********** estimate_ch_infos.c ********** */
int find_ch_map_entry(char *string, struct ch_map **map, int *num_entries);
int free_ch_map(struct ch_map **map, int num_entries);


/* ********** eval_auto.c ********** */
int create_auto_class(eval_handle eh, struct ra_auto_create_class *acc, value_handle *res, long res_offset);
int add_events(eval_handle eh, struct ra_auto_create_class *acc, value_handle *res, long res_offset);
int create_auto_prop(eval_handle eh, class_handle clh, struct ra_auto_create_class *acc,
		     struct ra_auto_create_prop *acp, value_handle *res, long res_offset);
int do_mass_insert(struct ra_auto_create_prop *acp, const long *ev_ids, long num_events, value_handle *res, long res_offset);


#ifdef __cplusplus
}
#endif

#endif /* _RA_PRIV_H */
