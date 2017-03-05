/*----------------------------------------------------------------------------
 * eval_dlg.cpp  -  a dialog showing all evaluations for a measurement
 *
 * Description:
 * The plugin provides a dialog which shows all evaluation of a measurement
 * which allows to delete evaluations and event-properties.
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2004, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/eval_dlg/eval_dlg.cpp,v 1.5 2004/06/30 09:53:23 rasch Exp $
 *--------------------------------------------------------------------------*/

#ifdef QT
#include "eval_dlg_qt.h"
#endif // _QT

#ifdef _AFXDLL
#include "stdafx.h"
#include "eval_dlg_mfc.h"
#endif // _AFXDLL

#include "eval_dlg.h"


LIBRAAPI int
load_ra_plugin(struct plugin_struct *ps)
{
	strcpy(ps->info.name, PLUGIN_NAME);
	strcpy(ps->info.desc, PLUGIN_DESC);
	sprintf(ps->info.build_ts, "%s  %s", __DATE__, __TIME__);
	ps->info.type = PLUGIN_TYPE;
	ps->info.license = PLUGIN_LICENSE;
	sprintf(ps->info.version, PLUGIN_VERSION);

	ps->info.num_results = 0;
	ps->info.num_options = 0;

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
	struct proc_info *pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->handle_id = RA_HANDLE_PROC;
	pi->plugin = pl;

	return pi;
} /* pl_get_proc_handle() */


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *)proc;
	
/*	if (pi->options)
		;*/
	       
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
	int ret = -1;
	struct proc_info *pi = (struct proc_info *) proc;

	if (!pi || !pi->mh)
		return ret;
#ifdef QT
	// if plugin is used in a non-Qt program, make the needed
	//  Qt environment
	int t = 0;
	QApplication *temp_app = NULL;
	if (!qApp)
		temp_app = new QApplication(t, NULL);

	eval_dlg_qt dlg(pi->mh, NULL);
	if (dlg.exec() == QDialog::Accepted)
		ret = 1;
	else
		ret = 0;

	// remove temporary created Qt environment
	if (temp_app)
		delete temp_app;
#endif // QT

#ifdef _AFXDLL
	struct plugin_struct *pl = (struct plugin_struct *) pi->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_EVAL_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	eval_dlg_mfc dlg;
	dlg.set_pi(pi);
	dlg.InitModalIndirect(p);
	if (dlg.DoModal() == IDOK)
		ret = 1;
	else
		ret = 0;

	FreeResource(g);
#endif // _AFXDLL

	return ret;
} /* pl_call_gui() */
