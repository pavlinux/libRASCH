/*----------------------------------------------------------------------------
 * simple_calc_dlg.cpp  -  dialog to perform calculations using process plugins
 *
 * Description:
 * The plugin provides a dialog to perform calculations done process plugins.
 * Only process plugins which return values (and not only store the results in
 * the evaluation file like the beat-detect plugin) can be used (and only these
 * can be selected).
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Header: /home/cvs_repos.bak/librasch/plugins/view/simple_calc_dlg/simple_calc_dlg.cpp,v 1.4 2004/06/30 09:53:24 rasch Exp $
 *--------------------------------------------------------------------------*/


#ifdef QT
#include "simple_calc_dlg_qt.h"
#endif // QT

#ifdef _AFXDLL
#include "stdafx.h"
#include "Resource.h"
#include "simple_calc_dlg_mfc.h"
#endif // _AFXDLL

#include "simple_calc_dlg.h"


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
	struct proc_info *pi = (struct proc_info *)ra_alloc_mem(sizeof(struct proc_info));
	if (!pi)
		return NULL;

	memset(pi, 0, sizeof(struct proc_info));
	pi->plugin = pl;

	return pi;
}  // pl_get_proc_handle()


void
pl_free_proc_handle(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *)proc;
	if (!pi)
		return;

	ra_free_mem(proc);
}  // pl_free_proc_handle()


int
pl_call_gui(proc_handle proc)
{
	struct proc_info *pi = (struct proc_info *) proc;

	if (!proc || !pi->mh)
	{
		_ra_set_error(proc, RA_ERR_INFO_MISSING, "plugin-simple-calc-dlg");
		return -1;
	}

	int ret = 0;
#ifdef QT
	int t = 0;
	// if plugin is used in a non-Qt program, make the needed
	//  Qt environment
	QApplication *temp_app = NULL;
	if (!qApp)
		temp_app = new QApplication(t, NULL);

	simple_calc_dlg_qt dlg(pi, NULL);
	if (dlg.exec() == QDialog::Rejected)
		ret = -1;

	// remove temporary created Qt environment
	if (temp_app)
		delete temp_app;

#endif // QT
#ifdef _AFXDLL
	struct plugin_struct *pl = (struct plugin_struct *) pi->plugin;
	HMODULE mod = LoadLibrary(pl->info.file);
	HRSRC res = FindResource(mod, (LPCTSTR) IDD_SIMPLE_CALC_DLG, RT_DIALOG);
	HGLOBAL g = LoadResource(mod, res);
	LPVOID p = LockResource(g);

	simple_calc_dlg_mfc dlg;
	dlg.set_pi(pi);
	dlg.InitModalIndirect(p);
	if (dlg.DoModal() != IDOK)
		ret = -1;
	FreeResource(g);
#endif // _AFXDLL

	return ret;
} /* pl_call_gui() */
