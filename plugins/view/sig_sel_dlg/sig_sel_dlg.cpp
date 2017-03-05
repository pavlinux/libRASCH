/*----------------------------------------------------------------------------
 * sig_sel_dlg.c  -  dialog to choose signales which can be handled with libRASCH
 *
 * Description:
 * The plugin provides a dialog which allows to choose a measurement. The dialog
 * shows all supported measurements in a directory, which can be selected.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>

#ifdef _AFXDLL
#include "stdafx.h"
#include "Resource.h"
#include "sig_sel_dlg_mfc.h"
#endif // _AFXDLL

#include "sig_sel_dlg.h"

#ifdef QT
#include <QtGui/QApplication>
#include <QtGui/QLabel>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include "sig_sel_dlg_qt.h"
#endif // QT

#include <ra_sig_sel.h>


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

	ps->call_gui = pl_call_gui;
	ps->proc.get_proc_handle = pl_get_proc_handle;
	ps->proc.free_proc_handle = pl_free_proc_handle;

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
	pi->options = calloc(1, sizeof(struct ra_sig_sel));
	set_option_offsets(ps->info.opt, (struct ra_sig_sel *)pi->options);
	set_default_options((struct ra_sig_sel *)pi->options);

	return pi;
} /* pl_get_proc_handle() */


void
set_default_options(struct ra_sig_sel *opt)
{
	opt->initial_path = NULL;
} /* set_default_options() */


void
set_option_offsets(struct ra_option_infos *opt_inf, struct ra_sig_sel *opt)
{
	unsigned char *p = (unsigned char *)opt;
	int idx = 0;

	opt_inf[idx++].offset = (long)((unsigned char *)&(opt->initial_path) - p);

	/* consistency check (useful when changing option structure) */
	if (idx != num_options)
	{
		fprintf(stderr, "critical error in sig_sel_slg.cpp - set_option_offsets():\n  number of options(%ld) != idx(%d)\n", num_options, idx);
		exit(-1);
	}
} /* set_option_offsets() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *)proc;
	
	if (pi->options)
	{
		struct ra_sig_sel *opt;

		opt = (struct ra_sig_sel *)pi->options;
		if (opt->initial_path)
			ra_free_mem(opt->initial_path);
		free(opt);
	}
	       
	if (pi->results)
	{
		int i;
		value_handle *t;

		t = pi->results;
		for (i = 0; i < (pi->num_results * pi->num_result_sets); i++)
			ra_value_free(t[i]);
		free(t);
	}

	ra_free_mem(proc);
} /* pl_free_proc_handle() */


int
pl_call_gui(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *) proc;

	if (!pi || (pi->ra == NULL))
		return -1;

	struct ra_sig_sel *opt = (struct ra_sig_sel *)pi->options;

	pi->num_result_sets = 1;
	pi->num_results = num_results;

	value_handle *res;
	res = (value_handle *)malloc(sizeof(value_handle *) * 
								 (pi->num_results * pi->num_result_sets));
	for (long l = 0; l < (pi->num_results * pi->num_result_sets); l++)
	{
		int curr_idx = l % pi->num_results;

		value_handle vh = ra_value_malloc();
		set_meta_info(vh, results[curr_idx].name, results[curr_idx].desc, RA_INFO_NONE);
		res[l] = vh;
	}
	pi->results = res;


#ifdef QT
	int t = 0;
	// if plugin is used in a non-Qt program, make the needed
	//  Qt environment
	QApplication *temp_app = NULL;
	if (!qApp)
		temp_app = new QApplication(t, NULL);

	sig_sel_dlg_qt dlg(pi->ra, opt->initial_path, NULL); //pi->view.parent);
	if (dlg.exec() == sig_sel_dlg_qt::Accepted)
	{
		QString s = dlg.get_signal(); 
		ra_value_set_string(res[0], s.toLocal8Bit().constData());
	}
	else
		ra_value_set_string(res[0], "");

	// remove temporary created Qt environment
	if (temp_app)
		delete temp_app;

#endif // QT
#ifdef _AFXDLL
	struct plugin_struct *pl = (struct plugin_struct *) pi->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC resource = FindResource(mod, (LPCTSTR) IDD_SEL_SIG_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, resource);
	LPVOID p = LockResource(g);

	sig_sel_dlg_mfc dlg;
	dlg.set_ra(pi->ra);
	dlg.InitModalIndirect(p);

	if (opt->initial_path)
	{
		dlg.set_savepath(opt->initial_path);
	}

	if (dlg.DoModal() == IDOK)
	{
		CString s = dlg.get_signal();
		ra_value_set_string(res[0], (const char *)s);
	}
	else
		ra_value_set_string(res[0], "");
	FreeResource(g);
#endif // _AFXDLL

	return 0;
} /* pl_call_gui() */

