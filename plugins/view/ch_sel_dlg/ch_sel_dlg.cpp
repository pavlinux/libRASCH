/**
 * \file ch_sel_dlg.cpp
 *
 * The plugin provides a dialog to select channels which will be used for
 * further processing.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*--------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifdef QT
#include "ch_sel_dlg_qt.h"
#endif // QT

#ifdef _AFXDLL
#include "stdafx.h"
#include "Resource.h"
#include "ch_sel_dlg_mfc.h"
#endif // _AFXDLL

#include "ch_sel_dlg.h"

#include <ra_ch_sel.h>


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	/* set result infos */
	ps->info.num_results = num_results;
	ps->info.res = (struct ra_result_infos *)ra_alloc_mem(sizeof(results));
	memcpy(ps->info.res, results, sizeof(results));

	/* set option infos */
	ps->info.num_options = num_options;
	ps->info.opt = (struct ra_option_infos *)ra_alloc_mem(sizeof(options));
	memcpy(ps->info.opt, options, sizeof(options));

	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;
	ps->call_gui = pl_call_gui;

	return 0;
} /* init_plugin() */


LIBRAAPI int
init_ra_plugin(struct ra *, struct plugin_struct *)
{
	return 0;
} /* init_ra_plugin() */


LIBRAAPI int
fini_ra_plugin(void)
{
	/* clean up */

	return 0;
} /* fini_plugin() */


proc_handle
pl_get_proc_handle(plugin_handle pl)
{
	struct plugin_struct *ps = (struct plugin_struct *)pl;
	struct proc_info *pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	/* init options */
	pi->options = calloc(1, sizeof(struct ra_ch_sel));
	set_option_offsets(ps->info.opt, (struct ra_ch_sel *)pi->options);
	set_default_options((struct ra_ch_sel *)pi->options);

	return pi;
}  // pl_get_proc_handle()


void
set_default_options(struct ra_ch_sel *opt)
{
	opt->num_ch = 0;
	opt->ch_type_filter = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_ch_sel *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->num_ch) - p);
	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->ch_type_filter) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in ch_sel_slg.cpp - set_option_offsets():\n"
			"  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *)proc;
	if (!pi)
		return;

	if (pi->options)
	{
		struct ra_ch_sel *opt;

		opt = (struct ra_ch_sel *)pi->options;
		if (opt->ch_type_filter)
			ra_free_mem(opt->ch_type_filter);
		free(opt);
	}

	if (pi->results)
	{
		int i;
		value_handle *t;

		t = (value_handle *)pi->results;
		for (i = 0; i < num_results; i++)
			ra_value_free(t[i]);
		free(t);
	}

	ra_free_mem(proc);
}  // pl_free_proc_handle()


int
pl_call_gui(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *) proc;

	if (!pi->rh)
		pi->rh = ra_rec_get_first(pi->mh, 0);

	if (!proc || !pi->mh || !pi->rh)
	{
		_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-ch-select-dlg");
		return -1;
	}

	/* init results */
	value_handle *res = (value_handle *)malloc(sizeof(value_handle *) * num_results);
	for (long i = 0; i < num_results; i++)
	{
		value_handle vh = ra_value_malloc();
		set_meta_info(vh, results[i].name, results[i].desc, RA_INFO_NONE);
		res[i] = vh;
	}
	pi->results = res;
	pi->num_result_sets = 1;

	int ret = 0;
#ifdef QT
	int t = 0;
	// if plugin is used in a non-Qt program, make the needed
	//  Qt environment
	QApplication *temp_app = NULL;
	if (!qApp)
		temp_app = new QApplication(t, NULL);

	ch_sel_dlg_qt dlg(pi, NULL);
	if (dlg.exec() == QDialog::Rejected)
		ret = -1;

	// remove temporary created Qt environment
	if (temp_app)
		delete temp_app;

#endif // QT
#ifdef _AFXDLL
	struct plugin_struct *pl = (struct plugin_struct *) pi->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_CH_SEL_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	ch_sel_dlg_mfc dlg;
	dlg.set_pi(pi);
	dlg.InitModalIndirect(p);
	if (dlg.DoModal() != IDOK)
		ret = -1;
	FreeResource(g);
#endif // _AFXDLL

	return ret;
} /* pl_call_gui() */

