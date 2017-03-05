/*----------------------------------------------------------------------------
 * simple_calc_dlg_qt.cpp
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2003-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QTextBrowser>
#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QCursor>

#include "simple_calc_dlg_qt.h"
#include "options_dlg_qt.h"


simple_calc_dlg_qt::simple_calc_dlg_qt(struct proc_info *pi, QWidget * parent/*=0*/)
	: QDialog(parent)
{
	setupUi(this);
	
	_pi = pi;
	_num_proc_pl = 0;
	_pi_for_pl = NULL;

	if (_pi->ra == NULL)
		_pi->ra = ra_lib_handle_from_any_handle(_pi->mh);

	value_handle vh = ra_value_malloc();
	if (ra_info_get(pi->ra, RA_INFO_NUM_PLUGINS_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}

	long num_pl = ra_value_get_long(vh);
	for (long l = 0; l < num_pl; l++)
	{
		plugin_handle pl = ra_plugin_get_by_num(pi->ra, l, 0);
		if (pl == NULL)
			continue;

		// only process plugins, which return results are used here
		if (ra_info_get(pl, RA_INFO_PL_TYPE_L, vh) != 0)
			continue;
		if (!(ra_value_get_long(vh) & PLUGIN_PROCESS))
			continue;
		
		if (ra_info_get(pl, RA_INFO_PL_NUM_RESULTS_L, vh) != 0)
			continue;
		if (ra_value_get_long(vh) <= 0)
			continue;

		if (ra_info_get(pl, RA_INFO_PL_NAME_C, vh) != 0)
			continue;
		pl_list->addItem(ra_value_get_string(vh));

		_num_proc_pl++;
		_pi_for_pl = (struct proc_info **)realloc(_pi_for_pl, sizeof(struct proc_info *) * _num_proc_pl);
		_pi_for_pl[_num_proc_pl - 1] = 	(struct proc_info *)ra_proc_get(pi->mh, pl, NULL);
	}
	ra_value_free(vh);

	pl_list->setCurrentIndex(0);

	QObject::connect(options_btn, SIGNAL(clicked()), this, SLOT(set_options()));
	QObject::connect(calc_btn, SIGNAL(clicked()), this, SLOT(calc()));
	QObject::connect(copy_btn, SIGNAL(clicked()), this, SLOT(copy_results_to_clipboard()));
	QObject::connect(exit_btn, SIGNAL(clicked()), this, SLOT(accept()));

#ifdef Q_WS_X11
	copy_btn->hide();
#endif // Q_WS_X11
} // constructor


simple_calc_dlg_qt::~simple_calc_dlg_qt()
{
	if (_pi_for_pl)
	{
		for (int i = 0; i < _num_proc_pl; i++)
		{
			ra_proc_free(_pi_for_pl[i]);
		}
		free(_pi_for_pl);
	}
} // destructor


void
simple_calc_dlg_qt::calc()
{
	res_list->clear();

	int idx = pl_list->currentIndex();
	if (idx < 0)
		return;
	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	if (ra_proc_do(_pi_for_pl[idx]) != 0)
	{
		QApplication::restoreOverrideCursor();
		return;
	}
	QApplication::restoreOverrideCursor();

        /* get number of result sets and results */
	value_handle vh = ra_value_malloc();
	if (ra_info_get(_pi_for_pl[idx], RA_INFO_PROC_NUM_RES_SETS_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n_res_set = ra_value_get_long(vh);

	if (ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_NUM_RESULTS_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n_res = ra_value_get_long(vh);

	results_text.clear();
	for (long l = 0; l < n_res_set; l++)
	{
		char out[200];
		sprintf(out, "result set %ld:", l+1);
		res_list->append(out);
		results_text += out;
		results_text += "\n";

		for (long m = 0; m < n_res; m++)
		{
			char t[100];
			
			/* test if result is a default value (some non-default results are
			   arrays which we skip) */
			ra_value_set_number(vh, m);
			ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_RES_DEFAULT_L, vh);
			if (ra_value_get_long(vh) == 0)
				continue;
			
			sprintf(out, "  ");
			ra_value_set_number(vh, m);
			if (ra_info_get(_pi_for_pl[idx]->plugin, RA_INFO_PL_RES_NAME_C, vh) == 0)
			{
				strcpy(t, ra_value_get_string(vh));
				strcat(out, t);
			}
			
			if (ra_proc_get_result(_pi_for_pl[idx], m, l, vh) == 0)
			{
				switch (ra_value_get_type(vh))
				{
				case RA_VALUE_TYPE_SHORT:
					sprintf(t, ": %d", ra_value_get_short(vh));
					break;
				case RA_VALUE_TYPE_LONG:
					sprintf(t, ": %ld", ra_value_get_long(vh));
					break;
				case RA_VALUE_TYPE_DOUBLE:
					sprintf(t, ": %f", ra_value_get_double(vh));
					break;
				default:
					strcpy(t, " - not supported type");
					break;
				}
				strcat(out, t);
			}
			
			res_list->append(out);

			results_text += out;
			results_text += "\n";
		}
	}
	ra_value_free(vh);
} // calc()


void
simple_calc_dlg_qt::set_options()
{
	int idx = pl_list->currentIndex();
	if (idx < 0)
		return;

	options_dlg_qt dlg(_pi_for_pl[idx], this);
	dlg.exec();	
} // set_options()


void
simple_calc_dlg_qt::copy_results_to_clipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(results_text);
 	qApp->processEvents();
} // copy_results_to_clipboard()
