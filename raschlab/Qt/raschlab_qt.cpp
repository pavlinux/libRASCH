/***************************************************************************
                          raschlab_qt.cpp  -  description
                             -------------------
    begin                : Sun Nov 4 2001
    copyright            : (C) 2001-2006 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Id$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Qt includes
//#include <QtGui>

// application specific includes
#include "raschlab_qt.h"
//#include "fileopen.xpm"
//#include "filesave.xpm"

#include <ra_ap_morphology.h>
#include <ra_fiducial_point.h>
#include <ra_template.h>
#include <ra_respiration.h>
#include <ra_plot_structs.h>
#include <ra_priv.h>
#include <libintl.h>
#include <pl_general.h>

#include "../general/defines.h"

#include "../../plugins/view/shared/progress.h"
detect_progress_dlg *dlg;


raschlab_qt::raschlab_qt(ra_handle ra)
{
	_ra = ra;
	if (ra_lib_get_error(_ra, NULL, 0) != RA_ERR_NONE)
	{
		char t[200];
		char err_txt[200];
		long err_num = ra_lib_get_error(_ra, t, 100);
		snprintf(err_txt, 200, gettext("Couldn't initialize libRASCH.\nReason: %s (%ld)"), t, err_num);
		QMessageBox::critical(0, "RASCHlab", err_txt);
		exit(-1);
	}

	QString s = "RASCHlab ";
	s += RASCHLAB_VERSION;
	setWindowTitle(s);

	// first read settings because some of these are needed when init menu entries
	_settings.read();


	workspace = new QWorkspace;
        setCentralWidget(workspace);
//	workspace->showMaximized();
//       connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(updateMenus()));
        window_mapper = new QSignalMapper(this);
        connect(window_mapper, SIGNAL(mapped(QWidget *)), workspace, SLOT(setActiveWindow(QWidget *)));

	// call inits to invoke all other construction parts
	create_actions();
        create_menus();
        create_toolbars();
        create_statusbar();	

//        updateMenus();

	if (_settings.is_maximized())
		showMaximized();
	else
	{
		if (_settings.pos_ok())
			move(_settings.pos_x(), _settings.pos_y());
		if (_settings.size_ok())
			resize(_settings.size_w(), _settings.size_h());
	}
	_settings.get_last_path(_last_path, 1024);

	view_toolbar->setChecked(true);
	view_statusbar->setChecked(true);

	view = NULL;
	dlg = new detect_progress_dlg(NULL);
}  // constructor


raschlab_qt::~raschlab_qt()
{
	delete dlg;
	ra_lib_close(_ra);
}  // destructor


void
raschlab_qt::create_actions()
{
	// -------------------- File menu actions --------------------
	file_open = new QAction(QString::fromUtf8(gettext("&Open...")), this);
	file_open->setShortcut(QString::fromUtf8(gettext("Ctrl+O")));
	file_open->setStatusTip(QString::fromUtf8(gettext("Opens a Signal handled by libRASCH")));
	file_open->setWhatsThis(QString::fromUtf8(gettext("Open Signal\n\nOpens a Signal handled by libRASCH")));
	connect(file_open, SIGNAL(triggered()), this, SLOT(slot_open()));

	file_save = new QAction(QString::fromUtf8(gettext("&Save")), this);
	file_save->setShortcut(QString::fromUtf8(gettext("Ctrl+S")));
	file_save->setStatusTip(QString::fromUtf8(gettext("Saves the changes done at the evaluation(s)")));
	file_save->setWhatsThis(QString::fromUtf8(gettext("Save Changes\n\nSaves the changes done at the evaluation(s)")));
	connect(file_save, SIGNAL(triggered()), this, SLOT(slot_save()));
	
	file_close = new QAction(QString::fromUtf8(gettext("&Close")), this);
	file_close->setShortcut(QString::fromUtf8(gettext("Ctrl+C")));
	file_close->setStatusTip(QString::fromUtf8(gettext("Closes the actual signal")));
	file_close->setWhatsThis(QString::fromUtf8(gettext("Close Signal\n\nCloses the actual signal")));
	connect(file_close, SIGNAL(triggered()), this, SLOT(slot_close()));
	
	file_quit = new QAction(QString::fromUtf8(gettext("E&xit")), this);
	file_quit->setShortcut(QString::fromUtf8(gettext("Ctrl+X")));
	file_quit->setStatusTip(QString::fromUtf8(gettext("Quits the application")));
	file_quit->setWhatsThis(QString::fromUtf8(gettext("Exit\n\nQuits the application")));
	connect(file_quit, SIGNAL(triggered()), this, SLOT(slot_quit()));

	// -------------------- General menu actions --------------------
	general_plugin = new QAction(QString::fromUtf8(gettext("&Plugin infos...")), this);
	general_plugin->setShortcut(QString::fromUtf8(gettext("Ctrl+P")));
	general_plugin->setStatusTip(QString::fromUtf8(gettext("Show infos about loaded plugins")));
	general_plugin->setWhatsThis(QString::fromUtf8(gettext("Plugin infos\n\nshow infos about loaded plugins")));
	connect(general_plugin, SIGNAL(triggered()), this, SLOT(slot_plugins()));

	general_eval = new QAction(QString::fromUtf8(gettext("&Evaluation infos...")), this);
	general_eval->setShortcut(QString::fromUtf8(gettext("Ctrl+E")));
	general_eval->setStatusTip(QString::fromUtf8(gettext("Show infos about evaluations")));
	general_eval->setWhatsThis(QString::fromUtf8(gettext("Evaluation infos\n\nshow infos about evaluations")));
	connect(general_eval, SIGNAL(triggered()), this, SLOT(slot_eval()));

	save_eval_ascii = new QAction(QString::fromUtf8(gettext("Save &ASCII eval-values")), this);
	save_eval_ascii->setShortcut(QString::fromUtf8(gettext("Ctrl+A")));
	save_eval_ascii->setStatusTip(QString::fromUtf8(gettext("Save the evaluation values as ASCII text")));
	save_eval_ascii->setWhatsThis(QString::fromUtf8(gettext("Save ASCII eval-values\n\nSave the evaluation values as ASCII text")));
	save_eval_ascii->setChecked(_settings.save_eval_ascii());
	connect(save_eval_ascii, SIGNAL(toggled(bool)), this, SLOT(slot_save_eval_ascii(bool)));

	// -------------------- Detect menu actions --------------------
	do_detect = new QAction(QString::fromUtf8(gettext("&Detect")), this);
	do_detect->setShortcut(QString::fromUtf8(gettext("Ctrl+D")));
	do_detect->setStatusTip(QString::fromUtf8(gettext("Runs libRASCH beat-detection")));
	do_detect->setWhatsThis(QString::fromUtf8(gettext("Detect\n\nRuns libRASCH beat-detection")));
	connect(do_detect, SIGNAL(triggered()), this, SLOT(slot_detect()));

	do_detect_ctg = new QAction(QString::fromUtf8(gettext("Detect C&TG")), this);
	do_detect_ctg->setShortcut(QString::fromUtf8(gettext("Ctrl+T")));
	do_detect_ctg->setStatusTip(QString::fromUtf8(gettext("Runs libRASCH CTG-detection")));
	do_detect_ctg->setWhatsThis(QString::fromUtf8(gettext("Detect\n\nRuns libRASCH CTG-detection")));
	connect(do_detect_ctg, SIGNAL(triggered()), this, SLOT(slot_detect_ctg()));

	do_rr_morph = new QAction(QString::fromUtf8(gettext("&RR-Morphology")), this);
	do_rr_morph->setShortcut(QString::fromUtf8(gettext("Ctrl+R")));
	do_rr_morph->setStatusTip(QString::fromUtf8(gettext("Runs libRASCH RR-Morphology Plugin")));
	do_rr_morph->setWhatsThis(QString::fromUtf8(gettext("RR-Morphology\n\nRuns libRASCH RR-Morphology Plugin")));
	connect(do_rr_morph, SIGNAL(triggered()), this, SLOT(slot_rr_morphology()));

	do_ecg_class = new QAction(QString::fromUtf8(gettext("&Classify ECG")), this);
	do_ecg_class->setShortcut(QString::fromUtf8(gettext("Ctrl+C")));
	do_ecg_class->setStatusTip(QString::fromUtf8(gettext("Runs libRASCH ecg Plugin")));
	do_ecg_class->setWhatsThis(QString::fromUtf8(gettext("Classify ECG\n\nRuns libRASCH ecg Plugin")));
	connect(do_ecg_class, SIGNAL(triggered()), this, SLOT(slot_classify_ecg()));

	do_calc = new QAction(QString::fromUtf8(gettext("&Perform Calculations")), this);
	do_calc->setShortcut(QString::fromUtf8(gettext("Ctrl+P")));
	do_calc->setStatusTip(QString::fromUtf8(gettext("Perform Calculations using libRASCH process plugins")));
	do_calc->setWhatsThis(QString::fromUtf8(gettext("Perform Calculations\n\nPerform Calculations using libRASCH process plugins")));
	connect(do_calc, SIGNAL(triggered()), this, SLOT(slot_calc()));

	// -------------------- View menu actions --------------------
	view_toolbar = new QAction(QString::fromUtf8(gettext("Tool&bar")), this);
	view_toolbar->setShortcut(QString::fromUtf8(gettext("Ctrl+B")));
	view_toolbar->setStatusTip(QString::fromUtf8(gettext("Enables/disables the toolbar")));
	view_toolbar->setWhatsThis(QString::fromUtf8(gettext("Toolbar\n\nEnables/disables the toolbar")));
	connect(view_toolbar, SIGNAL(toggled(bool)), this, SLOT(slot_show_toolbar(bool)));

	view_statusbar = new QAction(QString::fromUtf8(gettext("&Statusbar")), this);
	view_statusbar->setShortcut(QString::fromUtf8(gettext("Ctrl+S")));
	view_statusbar->setStatusTip(QString::fromUtf8(gettext("Enables/disables the statusbar")));
	view_statusbar->setWhatsThis(QString::fromUtf8(gettext("Statusbar\n\nEnables/disables the statusbar")));
	connect(view_statusbar, SIGNAL(toggled(bool)), this, SLOT(slot_show_statusbar(bool)));

	view_plot = new QAction(QString::fromUtf8(gettext("&Plot")), this);
	view_plot->setShortcut(QString::fromUtf8(gettext("Ctrl+P")));
	view_plot->setStatusTip(QString::fromUtf8(gettext("show RASCH-Plot")));
	view_plot->setWhatsThis(QString::fromUtf8(gettext("Plot\n\nshow RASCH-Plot")));
	connect(view_plot, SIGNAL(triggered()), this, SLOT(slot_show_plot()));

	// -------------------- Window menu actions --------------------
	window_new = new QAction(QString::fromUtf8(gettext("&New Window")), this);
	window_new->setShortcut(QString::fromUtf8(gettext("Ctrl+N")));
	window_new->setStatusTip(QString::fromUtf8(gettext("Opens a new view for the current document")));
	window_new->setWhatsThis(QString::fromUtf8(gettext("New Window\n\nOpens a new view for the current document")));
	connect(window_new, SIGNAL(triggered()), this, SLOT(slot_new_window()));

	window_cascade = new QAction(QString::fromUtf8(gettext("&Cascade")), this);
	window_cascade->setShortcut(QString::fromUtf8(gettext("Ctrl+C")));
	window_cascade->setStatusTip(QString::fromUtf8(gettext("Cascades all windows")));
	window_cascade->setWhatsThis(QString::fromUtf8(gettext("Cascade\n\nCascades all windows")));
	connect(window_cascade, SIGNAL(triggered()), workspace, SLOT(cascade()));

	window_tile = new QAction(QString::fromUtf8(gettext("&Tile")), this);
	window_tile->setShortcut(QString::fromUtf8(gettext("Ctrl+T")));
	window_tile->setStatusTip(QString::fromUtf8(gettext("Tiles all windows")));
	window_tile->setWhatsThis(QString::fromUtf8(gettext("Tile\n\nTiles all windows")));
	connect(window_tile, SIGNAL(triggered()), workspace, SLOT(tile()));

	// -------------------- Help menu actions --------------------
	help_about = new QAction(QString::fromUtf8(gettext("&About...")), this);
	help_about->setShortcut(QString::fromUtf8(gettext("Ctrl+A")));
	help_about->setStatusTip(QString::fromUtf8(gettext("About the application")));
	help_about->setWhatsThis(QString::fromUtf8(gettext("About\n\nAbout the application")));
	connect(help_about, SIGNAL(triggered()), this, SLOT(slot_about()));
} // create_actions()


void
raschlab_qt::create_menus()
{
	file_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&File")));
	file_menu->addAction(file_open);
	file_menu->addAction(file_save);
	file_menu->addSeparator();	
	file_menu->addAction(file_close);
	file_menu->addSeparator();	
	file_menu->addAction(file_quit);

	general_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&General")));
	general_menu->addAction(general_plugin);
	general_menu->addAction(general_eval);
	general_menu->addAction(save_eval_ascii);

	detect_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&Detect")));  // FIXME: will be generated dynamically
	detect_menu->addAction(do_calc);

	view_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&View")));
        view_menu->addAction(view_toolbar);
	view_menu->addAction(view_statusbar);
	view_menu->addSeparator();
	view_menu->addAction(view_plot);

	window_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&Window")));
	window_menu->addAction(window_new);
	window_menu->addAction(window_cascade);
	window_menu->addAction(window_tile);
	connect(window_menu, SIGNAL(aboutToShow()), this, SLOT(window_menu_about_to_show()));

	help_menu = menuBar()->addMenu(QString::fromUtf8(gettext("&Help")));
	help_menu->addAction(help_about);
	help_menu->addSeparator();	
//	help_menu->addAction(QString::fromUtf8(gettext("What's &This")), this, SLOT(whatsThis()), Qt::SHIFT+Qt::Key_F1);
} // create_menus()


void
raschlab_qt::create_toolbars()
{
	file_toolbar = addToolBar(QString::fromUtf8(gettext("File")));
 	file_toolbar->addAction(file_open);
	file_toolbar->addAction(file_save);
 	file_toolbar->addSeparator();
// 	QWhatsThis::whatsThisButton(file_toolbar);
}  // create_toolbars()


void
raschlab_qt::create_statusbar()
{
  statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // create_statusbar()


void
raschlab_qt::make_detect_menu(signal_ra_qt *sig)
{
	// first remove all menu entries and add do-calc entry which is always available
	detect_menu->clear();
	do_calc->setMenu(detect_menu);
	
	meas_handle mh = sig->get_meas_handle();
	rec_handle rec = ra_rec_get_first(mh, 0);

	value_handle vh = ra_value_malloc();
	if (ra_info_get(rec, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh) != 0)
	{
		ra_value_free(vh);
		return;
	}
	long n = ra_value_get_long(vh);
	bool ecg, rr, ctg;
	ecg = rr = ctg = false;
	for (long l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if (ra_info_get(rec, RA_INFO_REC_CH_TYPE_L, vh) != 0)
			continue;
	
		switch (ra_value_get_long(vh))
		{
		case RA_CH_TYPE_ECG:
			if (!ecg)
			{
				do_detect->setMenu(detect_menu);
				do_ecg_class->setMenu(detect_menu);
				ecg = true;
			}
			break;
		case RA_CH_TYPE_RR:
			if (!rr)
			{
				do_rr_morph->setMenu(detect_menu);
				rr = true;
			}
			break;
		case RA_CH_TYPE_CTG_UC:
			if (!ctg)
			{
				do_detect_ctg->setMenu(detect_menu);
				ctg = true;
			}
			break;
		}
	}
	ra_value_free(vh);
} // make_detect_menu()


signal_ra_qt *
raschlab_qt::active_signal()
{
	signal_ra_qt *sig = NULL;
	
	QWidget *v = (QWidget *)workspace->activeWindow();
	if(v)
	{
		QList<signal_ra_qt *>::iterator i;
		for (i = signal_list.begin(); i != signal_list.end(); ++i)
		{
			sig = *i;
			if (sig->find_view(v) >= 0)
				break;
		}
 	}
		
	return sig;
}  // active_signal()


void
raschlab_qt::open_signal_file(const char *file/*=0*/)
{
	if (!file)
		return;

	QCursor curs_save = cursor();
	setCursor(Qt::WaitCursor);

	signal_ra_qt *sig = new signal_ra_qt(_ra);
	if (!sig->open(file))
	{
		setCursor(curs_save);
		delete sig;
		return;
	}	
	signal_list.append(sig);
	create_view(sig);

	setCursor(curs_save);

	make_detect_menu(sig);
}  // open_signal_file()

void
raschlab_qt::slot_open()
{
	statusBar()->showMessage(QString::fromUtf8(gettext("Opening file...")));

	QString fn = select_signal();
	if (fn.isEmpty())
		return;
				
	// check, if signal is already open
/* Jesus means that it would be good to be able to open same signal more than once
	signal_ra *sig;
	for(sig = signal_list->first(); sig > 0; sig = signal_list->next())
	{
		char curr_fn[MAX_PATH_RA];
		sig->file_name(curr_fn, MAX_PATH_RA);
		if(strcmp(curr_fn, fn) == 0)
		{
			// already open -> set focus to first view of signal
			QWidget *v = (QWidget *)(sig->get_view(NULL));
			v->setFocus();
			return;
		}
	}
*/
	open_signal_file(fn.toLocal8Bit().data());
	
	statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // slot_open()


QString
raschlab_qt::select_signal()
{
	QString sig = "";
	
	plugin_handle plugin = ra_plugin_get_by_name(_ra, "sig-sel-dlg", 1);
	if (!plugin)
		return sig;
		
	struct proc_info *pi = (struct proc_info *)ra_proc_get(NULL, plugin, NULL);
	value_handle vh = ra_value_malloc();
	ra_value_set_string(vh, (const char *)_last_path);
	ra_lib_set_option(pi, "initial_path", vh);

	ra_gui_call(pi, plugin);

	if (ra_proc_get_result(pi, 0, 0, vh) == 0)
		sig = ra_value_get_string(vh);
	ra_value_free(vh);

	ra_proc_free(pi);

	strcpy(_last_path, sig.toLocal8Bit().data());
	char *p = strrchr(_last_path, '/');
	if (p)
		*p = '\0';

	return sig;	
}  // select_signal()


void
raschlab_qt::create_view(signal_ra_qt *sig)
{
	// FIXME: type of view will be read from some config-files or similar
	plugin_handle p = ra_plugin_get_by_name(_ra, "rasch-view", 1);
	if (!p)
		return;

	char title[100];
	sig->get_title(title, 100);
	
	view_handle vih = ra_view_get(sig->get_meas_handle(), p, NULL);
	if (!vih)
		return;
	if (ra_view_create(vih) != 0)
	{
		ra_view_free(vih);
		return;
	}

 	QWidget *rec_view = (QWidget *)ra_view_get_window(vih, 0);
	if (!rec_view)
	{
		ra_view_free(vih);
		return;
	}
        workspace->addWindow(rec_view);

	QString t = QString::fromUtf8(gettext("recording: "));
	t += title;
	rec_view->setWindowTitle(t);
	sig->add_view(rec_view, vih, SIGNAL_VIEW);
	
	QWidget *plot_view= NULL;
	eval_handle eh = ra_eval_get_default(sig->get_meas_handle());
	if (!eh)
	{
		rec_view->showMaximized();
		return;		
	}

	// decide if template window is shown (and with which infos)
	QWidget *templ_view = NULL;
	value_handle vh = ra_value_malloc();
	ra_class_get(eh, "heartbeat", vh);
	if (ra_value_get_num_elem(vh) > 0)
	{
		class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
		ra_sum_get(clh, "template", vh);
		if (ra_value_get_num_elem(vh) > 0)
			templ_view = show_qrs_templates(sig, (sum_handle)(ra_value_get_voidp_array(vh)[0]));
	}

	ra_class_get(eh, "uterine-contraction", vh);
	if (ra_value_get_num_elem(vh) > 0)
	{
		class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
		ra_sum_get(clh, "template", vh);
		if (ra_value_get_num_elem(vh) > 0)
			templ_view = show_uc_templates(sig, (sum_handle)(ra_value_get_voidp_array(vh)[0]));
	}
	ra_value_free(vh);

	p = ra_plugin_get_by_name(_ra, "plot-view", 1);
	if (p)
	{
		struct ra_plot_options *opt = (struct ra_plot_options *)ra_alloc_mem(sizeof(struct ra_plot_options));
		bool ok = fill_plot_infos(sig, opt);
		if (ok)
		{
			vih = ra_view_get(sig->get_meas_handle(), p, workspace);
			// TODO: use set-option functions when they are working with all view plugins
			struct view_info *vi = (struct view_info *)vih;
			vi->options = opt;
			ra_view_create(vih);
			plot_view = (QWidget *)ra_view_get_window(vih, 0);
			t = "plot: ";
			t += title;
			plot_view->setWindowTitle(t);
			sig->add_view(plot_view, vih, PLOT_VIEW);
		}
		else
			ra_free_mem(opt);
	}
	
	if (!plot_view && !templ_view)
		rec_view->showMaximized();
	else
	{
		rec_view->show();
		if (plot_view)
			plot_view->show();
		if (templ_view)
			templ_view->show();
	}
		rec_view->showMaximized();

	resizeEvent(NULL);
}  // create_view()


QWidget *
raschlab_qt::show_qrs_templates(signal_ra_qt *sig, sum_handle sh)
{
 	plugin_handle p = ra_plugin_get_by_name(_ra, "ev-summary-view", 1);
 	if (!p)
 		return NULL;
	
	view_handle vih = ra_view_get(sig->get_meas_handle(), p, workspace);
	if (vih == NULL)
		return NULL;

	value_handle vh = ra_value_malloc();
	ra_value_set_voidp(vh, sh);
	ra_lib_set_option(vih, "sh", vh);
	ra_value_free(vh);

	if (ra_view_create(vih) != 0)
	{
		ra_view_free(vih);
		return NULL;
	}
	
 	QWidget *view = (QWidget *)ra_view_get_window(vih, 0);
 	if (!view)
	{
		ra_view_free(vih);
 		return NULL;
	}
 	QString t = QString::fromUtf8(gettext("qrs-templates: "));
 	char title[100];
 	sig->get_title(title, 100);
 	t += title;
 	view->setWindowTitle(t);
 	sig->add_view(view, vih, TEMPLATE_VIEW);
	
 	return view;
}  // show_qrs_templates()


QWidget *
raschlab_qt::show_uc_templates(signal_ra_qt *sig, sum_handle sh)
{
 	plugin_handle p = ra_plugin_get_by_name(_ra, "ev-summary-view", 1);
 	if (!p)
 		return NULL;
	
	view_handle vih = ra_view_get(sig->get_meas_handle(), p, workspace);
	if (vih == NULL)
		return NULL;

	value_handle vh = ra_value_malloc();
	ra_value_set_voidp(vh, sh);
	ra_lib_set_option(vih, "sh", vh);
	ra_value_free(vh);

	if (ra_view_create(vih) != 0)
	{
		ra_view_free(vih);
		return NULL;
	}
	
 	QWidget *view = (QWidget *)ra_view_get_window(vih, 0);
 	if (!view)
	{
		ra_view_free(vih);
 		return NULL;
	}
 	QString t = QString::fromUtf8(gettext("uc-templates: "));
 	char title[100];
 	sig->get_title(title, 100);
 	t += title;
 	view->setWindowTitle(t);
 	sig->add_view(view, vih, TEMPLATE_VIEW);
	
 	return view;
}  // show_uc_templates()


void
raschlab_qt::slot_save()
{
	signal_ra *sig = active_signal();
	if (!sig)
		return;

	sig->save((int)_settings.save_eval_ascii());		
}  // slot_save()

	
void
raschlab_qt::slot_close()
{
	detect_menu->clear();
	detect_menu->addAction(do_calc);

	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;
}  // slot_close()


void
raschlab_qt::slot_quit()
{
	// close all signals
	while (!signal_list.empty())
	{
		signal_ra_qt *sig = signal_list.first();
		signal_list.removeFirst();
		delete sig;
	}

	statusBar()->showMessage(QString::fromUtf8(gettext("Exiting application...")));

	_settings.set_maximized(isMaximized());
	QPoint p = pos();
	_settings.set_pos_x(p.x());
	_settings.set_pos_y(p.y());
	QSize s = size();
	_settings.set_size_w(s.width());
	_settings.set_size_h(s.height());
	_settings.set_last_path(_last_path);

	_settings.save();

	qApp->quit();

	statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // slot_quit


void
raschlab_qt::slot_plugins()
{
	plugin_handle p = ra_plugin_get_by_name(_ra, "plugin-info-dlg", 1);
	if (!p)
		return;

	struct proc_info pi;
	memset(&pi, 0, sizeof(struct proc_info));
	pi.handle_id = RA_HANDLE_PROC;
	pi.ra = _ra;
	pi.plugin = p;
	ra_gui_call(&pi, pi.plugin);
}  // slot_plugins()


void
raschlab_qt::slot_eval()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;
	
	plugin_handle p = ra_plugin_get_by_name(_ra, "eval-dlg", 1);
	if (!p)
		return;
			
	struct proc_info pi;
	pi.mh = mh;
	pi.plugin = p;
 	if (ra_gui_call(&pi, pi.plugin) == 1) // if 'OK', save changes
	{
 		ra_eval_save(mh, NULL, (int)_settings.save_eval_ascii());

		char fn[MAX_PATH_RA];
		sig->file_name(fn, MAX_PATH_RA);

		sig->close();
		int idx = signal_list.indexOf(sig);
		signal_list.removeAt(idx);
		delete sig;
		
		open_signal_file(fn);
	}
}  // slot_eval()


void
raschlab_qt::slot_save_eval_ascii(bool toggle)
{
	statusBar()->showMessage(QString::fromUtf8(gettext("Toggle that eval-values are saved in ASCII...")));

	_settings.set_save_eval_ascii(toggle);

	statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // slot_save_eval_ascii()


// FIXME: will be generated dynamically, later
void
detect_callback(const char *text, int percent)
{
	if (text)
		dlg->set_text(text);
	dlg->set_progress(percent);
	
	qApp->processEvents();
}  // detect_callback


void
raschlab_qt::slot_detect()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	struct ra_ch_list ch_list;
	memset(&ch_list, 0, sizeof(struct ra_ch_list));

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "ch-select-dlg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		int ret = ra_gui_call(pi, pi->plugin);
		if (ret != 0)
		{
			ra_proc_free(pi);
			return;
		}

		value_handle vh = ra_value_malloc();
		ra_proc_get_result(pi, 0, 0, vh);
		ch_list.num_ch = ra_value_get_num_elem(vh);
		ch_list.ch = new long[ch_list.num_ch];
		memcpy(ch_list.ch, ra_value_get_long_array(vh), sizeof(long) * ch_list.num_ch);
		ra_value_free(vh);

		ra_proc_free(pi);
	}

	dlg->show();	
	qApp->processEvents();

	/* prepare eval to store the detection results */
	eval_handle eh_save = ra_eval_get_default(mh);
	eval_handle eh = ra_eval_add(mh, "RASCHlab", "evaluation created within RASCHlab", 0);
	if (!eh)
	{
		dlg->hide();
		delete[] ch_list.ch;
		return;
	}
	
	if ((pl = ra_plugin_get_by_name(_ra, "detect-ecg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long_array(vh, ch_list.ch, ch_list.num_ch);
		ra_lib_set_option(pi, "ch", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_eval", vh);
		ra_value_set_voidp(vh, eh);
		ra_lib_set_option(pi, "eh", vh);
 		ra_value_set_long(vh, 1);
 		ra_lib_set_option(pi, "do_interpolation", vh);

		ra_value_free(vh);

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

	value_handle vh = ra_value_malloc();
	if (ra_class_get(eh, "heartbeat", vh) != 0)
	{
		dlg->hide();
		delete[] ch_list.ch;
		ra_eval_delete(eh);
		if (eh_save)
			ra_eval_set_default(eh_save);
		return;
	}
	class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	ra_value_free(vh);

	if ((pl = ra_plugin_get_by_name(_ra, "fiducial-point", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(pi, "clh", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_class", vh);
		ra_value_set_long(vh, ch_list.ch[0]);
		ra_lib_set_option(pi, "ch", vh);

		ra_value_free(vh);

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

 	if ((pl = ra_plugin_get_by_name(_ra, "template", 1)) != NULL)
 	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

 		value_handle vh = ra_value_malloc();

 		ra_value_set_long(vh, 1);
 		ra_lib_set_option(pi, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(pi, "clh", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_class", vh);
 		ra_value_set_string(vh, "qrs-pos");
 		ra_lib_set_option(pi, "pos_name", vh);
 		ra_value_set_string(vh, "qrs-template");
 		ra_lib_set_option(pi, "templ_name", vh);
 		ra_value_set_string(vh, "qrs-template-corr");
 		ra_lib_set_option(pi, "templ_corr", vh);
 		ra_value_set_double(vh, 0.85);
 		ra_lib_set_option(pi, "accept", vh);
 		ra_value_set_double(vh, 0.6);
 		ra_lib_set_option(pi, "slope_accept_low", vh);
 		ra_value_set_double(vh, 1.4);
 		ra_lib_set_option(pi, "slope_accept_high", vh);
 		ra_value_set_double(vh, 0.15);
 		ra_lib_set_option(pi, "win_before", vh);
 		ra_value_set_double(vh, 0.15);
 		ra_lib_set_option(pi, "win_after", vh);
 		ra_value_set_long_array(vh, ch_list.ch, ch_list.num_ch);
 		ra_lib_set_option(pi, "ch", vh);

 		ra_value_free(vh);

 		ra_proc_do(pi);
 		ra_proc_free(pi);
 	}

 	if ((pl = ra_plugin_get_by_name(_ra, "ecg", 1)) != NULL)
 	{
 		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(pi, "clh", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_class", vh);

		ra_value_free(vh);

 		ra_proc_do(pi);
 		ra_proc_free(pi);
 	}

 	if ((pl = ra_plugin_get_by_name(_ra, "ap-morphology", 1)) != NULL)
 	{
 		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(pi, "clh", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_class", vh);
 		ra_value_set_long(vh, 0);
 		ra_lib_set_option(pi, "rr_pos", vh);
 		ra_lib_set_option(pi, "dont_search_calibration", vh);

		ra_value_free(vh);

 		ra_proc_do(pi);
 		ra_proc_free(pi);
 	}

// 	if ((pl = ra_plugin_get_by_name(_ra, "respiration", 1)) != NULL)
// 	{
// 		struct proc_info *pi = (struct proc_info *)ra_proc_get(pl);
// 		pi->plugin = pl;
// 		pi->mh = mh;
// 		pi->rh = ra_rec_get_first(mh, 0);
// 		pi->eh = eh;
// 		pi->save_in_eval = 1;
// 		pi->callback = detect_callback;

// 		value_handle vh = ra_value_malloc();
// 		ra_value_set_long(vh, 1);
// 		ra_lib_set_option(pi, "use_eval", vh);
// 		ra_value_set_long(vh, (long)pi->rh);
// 		ra_lib_set_option(pi, "rh", vh);
// 		ra_value_set_long(vh, (long)eh);
// 		ra_lib_set_option(pi, "eh", vh);
// 		ra_value_set_long(vh, -1);
// 		ra_lib_set_option(pi, "ch_num", vh);
// 		ra_value_free(vh);

// 		ra_proc_do(pi);
// 		ra_proc_free(pi);
// 	}

	detect_callback("save evaluation file", 0);	
 	ra_eval_save(mh, NULL, (int)_settings.save_eval_ascii());
		
	dlg->hide();
	delete[] ch_list.ch;

	char fn[MAX_PATH_RA];
	sig->file_name(fn, MAX_PATH_RA);

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;

	open_signal_file(fn);
}  // slot_detect()


void
raschlab_qt::slot_detect_ctg()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	struct ra_ch_list ch_list;
	memset(&ch_list, 0, sizeof(struct ra_ch_list));

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "ch-select-dlg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		int ret = ra_gui_call(pi, pi->plugin);
		if (ret != 0)
		{
			ra_proc_free(pi);
			return;
		}

		value_handle vh = ra_value_malloc();
		ra_proc_get_result(pi, 0, 0, vh);
		ch_list.num_ch = ra_value_get_num_elem(vh);
		ch_list.ch = new long[ch_list.num_ch];
		memcpy(ch_list.ch, ra_value_get_long_array(vh), sizeof(long) * ch_list.num_ch);
		ra_value_free(vh);

		ra_proc_free(pi);
	}

		
 	dlg->show();	
 	qApp->processEvents();
	
	/* prepare eval to store the detection results */
	eval_handle eh_save = ra_eval_get_default(mh);
	eval_handle eh = ra_eval_add(mh, "RASCHlab", "evaluation created within RASCHlab", 0);
	if (!eh)
	{
		dlg->hide();
		delete[] ch_list.ch;
		return;
	}
	
	if ((pl = ra_plugin_get_by_name(_ra, "detect-ctg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long_array(vh, ch_list.ch, ch_list.num_ch);
		ra_lib_set_option(pi, "ch", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_eval", vh);
		ra_value_set_voidp(vh, eh);
		ra_lib_set_option(pi, "eh", vh);

		ra_value_free(vh);

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

	value_handle vh = ra_value_malloc();
	if (ra_class_get(eh, "uterine-contraction", vh) != 0)
	{
		dlg->hide();
		delete[] ch_list.ch;
		ra_eval_delete(eh);
		if (eh_save)
			ra_eval_set_default(eh_save);
		return;
	}
	class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	ra_value_free(vh);

 	if ((pl = ra_plugin_get_by_name(_ra, "template", 1)) != NULL)
 	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

 		value_handle vh = ra_value_malloc();

 		ra_value_set_long(vh, 1);
 		ra_lib_set_option(pi, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(pi, "clh", vh);
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "save_in_class", vh);
		ra_value_set_string(vh, "uc-max-pos");
		ra_lib_set_option(pi, "pos_name", vh);
		ra_value_set_string(vh, "uc-template");
		ra_lib_set_option(pi, "templ_name", vh);
		ra_value_set_string(vh, "uc-template-corr");
		ra_lib_set_option(pi, "templ_corr", vh);
		ra_value_set_double(vh, 0.85);
		ra_lib_set_option(pi, "accept", vh);
		ra_value_set_double(vh, 0.6);
		ra_lib_set_option(pi, "slope_accept_low", vh);
		ra_value_set_double(vh, 1.4);
		ra_lib_set_option(pi, "slope_accept_high", vh);
		ra_value_set_double(vh, 40);
		ra_lib_set_option(pi, "win_before", vh);
		ra_value_set_double(vh, 40);
		ra_lib_set_option(pi, "win_after", vh);
		ra_value_set_long_array(vh, ch_list.ch, ch_list.num_ch);
		ra_lib_set_option(pi, "ch", vh);

 		ra_value_free(vh);

 		ra_proc_do(pi);
 		ra_proc_free(pi);
 	}

	detect_callback("save evaluation file", 0);	
 	ra_eval_save(mh, NULL, (int)_settings.save_eval_ascii());
		
	dlg->hide();
	delete[] ch_list.ch;

	char fn[MAX_PATH_RA];
	sig->file_name(fn, MAX_PATH_RA);

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;

	open_signal_file(fn);
}  // slot_detect_ctg()


void
raschlab_qt::slot_rr_morphology()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	char fn[MAX_PATH_RA];
	sig->file_name(fn, MAX_PATH_RA);

	process_signal("ap-morphology");

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;
	open_signal_file(fn);
}  // slot_rr_morphology()


void
raschlab_qt::process_signal(char *plugin_name)
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;

	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	eval_handle eh = ra_eval_get_default(mh);
    	if (!eh)
        	return;

	dlg->show();
	qApp->processEvents();

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, plugin_name, 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

	dlg->hide();
}  // process_signal()


void
raschlab_qt::slot_classify_ecg()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	char fn[MAX_PATH_RA];
	sig->file_name(fn, MAX_PATH_RA);

	process_signal("ecg");

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;
	open_signal_file(fn);
}  // slot_classify_ecg()


void
raschlab_qt::slot_calc()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "simple-calc-dlg", 1)) == NULL)
		return;

	struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, NULL);
	ra_gui_call(pi, pl);
	ra_proc_free(pi);
}  // slot_calc()


void
raschlab_qt::slot_show_toolbar(bool toggle)
{
	statusBar()->showMessage(QString::fromUtf8(gettext("Toggle toolbar...")));

	if (toggle== false)
		file_toolbar->hide();
	else
		file_toolbar->show();

	statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // slot_show_toolbar()


void
raschlab_qt::slot_show_statusbar(bool toggle)
{
	statusBar()->showMessage(QString::fromUtf8(gettext("Toggle statusbar...")));

	if (toggle == false)
		statusBar()->hide();
	else
		statusBar()->show();

	statusBar()->showMessage(QString::fromUtf8(gettext("Ready.")));
}  // slot_show_statusbar()


void
raschlab_qt::slot_show_plot()
{
	return;

// 	signal_ra_qt *sig = active_signal();
// 	if (!sig)
// 		return;
	
// 	meas_handle mh = sig->get_meas_handle();
// 	if (!mh)
// 		return;
	
// 	plugin_handle p = ra_plugin_get_by_name(_ra, "plot-view", 1);
// 	if (!p)
// 		return;

// 	view_handle vih = ra_view_get(sig->get_meas_handle(), p, workspace);

// 	struct view_info vi;
// 	memset(&vi, 0, sizeof(struct view_info));
// 	vi.parent = workspace;
// 	vi.mh = mh;
// 	vi.rh = ra_rec_get_first(mh, 0);
//         if (!ra_eval_get_default(mh))
//         	return;
	
//  	if ((vi.plugin = ra_plugin_get_by_name(_ra, "plot-view", 1)) != NULL)
//  	{
//  		int ret = ra_gui_call(&vi, vi.plugin);
//  		if (ret != 0)
//  			return;
		
//  		if (ra_create_view(vih) != 0)
//  			return;
		
//  		QWidget *view = (QWidget *)(vi.views[0]);
//  		if (!view)
//  			return;
//  		QString t = "plot: ";
//  		view->setCaption(t);
//  		sig->add_view(view, PLOT_VIEW);
		
//  		view->show();
// 	}
	
//  	resizeEvent(NULL);
}  // slot_show_plot()


void
raschlab_qt::slot_new_window()
{
}  // slot_new_window()


void
raschlab_qt::window_menu_about_to_show()
{
	window_menu->clear();
	window_menu->addAction(window_new);
	window_menu->addAction(window_cascade);
	window_menu->addAction(window_tile);
/*	
	if (workspace->windowList().isEmpty())
	{
		window_action->setEnabled(false);
	}
	else
	{
		window_action->setEnabled(true);
	}

	window_menu->addSeparator();
*/
/*	QWidgetList windows = workspace->windowList();
	for (int i = 0; i < int(windows.count()); ++i)
	{
		QAction *a = window_menu->addAction(QString("&%1 ").arg(i+1)+windows.at(i)->windowTitle(),
						    this, SLOT(window_menu_activated(int)));		
//		window_menu->setItemParameter( id, i );
//		window_menu->setItemChecked( id, workspace->activeWindow() == windows.at(i) );
}*/
}  // window_menu_about_to_show()


void
raschlab_qt::window_menu_activated(int id)
{
	QWidget* w = workspace->windowList().at(id);
	if (w)
		w->setFocus();
}  // window_menu_activated()


void
raschlab_qt::slot_about()
{
	value_handle vh = ra_value_malloc();
	ra_info_get(_ra, RA_INFO_VERSION_C, vh);
	QString s = "Raschlab Version 0.2.4\n(c) 2002-2005 by Raphael Schneider (rasch@med1.med.tum.de)\nusing libRASCH Version ";
	s += ra_value_get_string(vh);
	ra_value_free(vh);
	QMessageBox::about(this, QString::fromUtf8(gettext("About...")), s);
}  // slot_about


void
raschlab_qt::resizeEvent(QResizeEvent *)
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;

	int n = sig->num_views();	

	if (n <= 1)
		return;  // is alreay maximized

	bool no_template_view = true;
	QWidget *v = NULL;
	for (int i = 0; i < n; i++)
	{
		v = (QWidget *)sig->get_view(v);
		if (!v)
			break;
		
		if (sig->get_view_type(v) == TEMPLATE_VIEW)
		{
			no_template_view = false;
			break;
		}
	}

	QRect r = workspace->geometry();
 	int pv_start, pv_height;
 	if (sig->num_plot_views() > 0)
 	{
 		pv_start = (int)((double)(r.height()) * (sig->num_plot_views() >= 2 ? 0.33 : 0.66));
 		pv_height = (r.height() - pv_start) / sig->num_plot_views();
 	}

	v = NULL;
	int num_plot_view_shown = 0;
	for (int i = 0; i < n; i++)
	{
		v = (QWidget *)sig->get_view(v);
		if (!v)
			break;
		
		int type = sig->get_view_type(v);
		int x, y, w, h;
		bool dont_move = false;
		switch (type)
		{
		case SIGNAL_VIEW:
			x = y = 0;
			w = r.width();
			if (sig->num_plot_views() >= 2)
				h = (int)((double)(r.height()) * 0.33);
			else
				h = (int)((double)(r.height()) * 0.66);
			break;
		case TEMPLATE_VIEW:
			x = 0;
			y = (int)((double)(r.height()) * 0.66);
			if (sig->num_plot_views() == 0)
				w = r.width();
			else
				w = (int)((double)(r.width()) * 0.33);
			h = r.height() - y;
			break;
		case PLOT_VIEW:
			if (no_template_view)
				x = 0;
			else
				x = (int)((double)(r.width()) * 0.33);
			w = r.width() - x;
			y = pv_start + (num_plot_view_shown * pv_height);
			h = pv_height;
			num_plot_view_shown++;
			break;
		default:
			dont_move = true;
			break;
		}
		if (dont_move)
			continue;
			
		v->parentWidget()->setGeometry(x, y, w, h);
	}
	sig->update_all_views(NULL);
}  // slot_resize()


bool
raschlab_qt::fill_plot_infos(signal_ra_qt *sig, struct ra_plot_options *opt)
{
//	return false;

 	memset(opt, 0, sizeof(struct ra_plot_options));

 	eval_handle eval = ra_eval_get_default(sig->get_meas_handle());
	
	value_handle vh = ra_value_malloc();
	ra_class_get(eval, "heartbeat", vh);
	if (ra_value_get_num_elem(vh) <= 0)
	{
		ra_value_free(vh);
		return false;
	}
	opt->eval = eval;
	opt->clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
 	strcpy(opt->class_name, "heartbeat");
	
 	prop_handle pos = ra_prop_get(opt->clh, "qrs-pos");
  	prop_handle rri = ra_prop_get(opt->clh, "rri");
   	prop_handle syst = ra_prop_get(opt->clh, "rr-systolic");
    	prop_handle dias = ra_prop_get(opt->clh, "rr-diastolic");
    	prop_handle resp = ra_prop_get(opt->clh, "resp-chest-mean-rri");

 	opt->show_legend = 1;

 	if (!pos)
 		return false;

 	add_pos_axis(opt, pos);
 	bool some_added = false;
 	if (rri)
 	{
 		add_rri_axis(opt, rri);
 		add_pair(opt, 0, (opt->num_axis-1), "RR-interval", 0, 0, 0);
 		some_added = true;
 	}
 	if (syst)
 	{
 		add_syst_axis(opt, syst);
 		add_pair(opt, 0, (opt->num_axis-1), "Systolic", 255, 0, 0);
 		some_added = true;
 	}
 	if (dias)
 	{
 		add_dias_axis(opt, dias);
 		add_pair(opt, 0, (opt->num_axis-1), "Diastolic", 0, 0, 255);
 		some_added = true;
 	}
 	if (resp)
 	{
 		add_resp_axis(opt, resp);
 		add_pair(opt, 0, (opt->num_axis-1), "Respiration", 0, 255, 0);
 		some_added = true;
 	}

 	return some_added;
}  // fill_plot_infos()


struct ra_plot_axis *
raschlab_qt::add_axis(struct ra_plot_options *opt)
{
	opt->num_axis++;
	struct ra_plot_axis *t = (struct ra_plot_axis *)ra_alloc_mem(sizeof(struct ra_plot_axis) * opt->num_axis);
	memset(t, 0, sizeof(struct ra_plot_axis) * opt->num_axis);
	if (opt->axis)
	{
		memcpy(t, opt->axis, sizeof(struct ra_plot_axis) * (opt->num_axis-1));
		ra_free_mem(opt->axis);
	}
	opt->axis = t;

	return &(opt->axis[opt->num_axis-1]);
}  // add_axis()


void
raschlab_qt::add_pos_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "qrs-pos");
	curr->event_property = prop;
	strcpy(curr->unit, "min");
	curr->is_time = 1;
	curr->time_format = RA_PLOT_TIME_MIN;
	curr->ch = -1;
}  // add_pos_axis()


void
raschlab_qt::add_rri_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "rri");
	curr->event_property = prop;
	strcpy(curr->unit, "ms");
	curr->use_min = curr->use_max = 1;
	curr->min = 200;
	curr->max = 2000;
	curr->ch = -1;
}  // add_rr_axis()


void
raschlab_qt::add_syst_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "rr-systolic");
	curr->event_property = prop;
	strcpy(curr->unit, "mmHg");
	curr->use_min = curr->use_max = 1;
	curr->min = 0;
	curr->max = 200;

	// bloodpressure values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh, RA_CH_TYPE_RR, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_syst_axis()


void
raschlab_qt::add_dias_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "rr-diastolic");
	curr->event_property = prop;
	strcpy(curr->unit, "mmHg");
	curr->use_min = curr->use_max = 1;
	curr->min = 0;
	curr->max = 200;

	// bloodpressure values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh, RA_CH_TYPE_RR, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_dias_axis()


void
raschlab_qt::add_resp_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "resp-chest-mean-rri");
	curr->event_property = prop;
	strcpy(curr->unit, "%");
	curr->use_min = curr->use_max = 1;
	curr->min = -1;
	curr->max = 1;

	// respiration values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh, RA_CH_TYPE_RESP, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_resp_axis()


void
raschlab_qt::add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, char *name,
	long r, long g, long b)
{
	opt->num_plot_pairs++;
	struct ra_plot_pair *t = (struct ra_plot_pair *)ra_alloc_mem(sizeof(struct ra_plot_pair) * opt->num_plot_pairs);
	memset(t, 0, sizeof(struct ra_plot_pair) * opt->num_plot_pairs);
	if (opt->pair)
	{
		memcpy(t, opt->pair, sizeof(struct ra_plot_pair) * (opt->num_plot_pairs-1));
		ra_free_mem(opt->pair);
	}
	opt->pair = t;
	
	struct ra_plot_pair *curr = opt->pair + (opt->num_plot_pairs-1);
	curr->x_idx = x_idx;
	curr->y_idx = y_idx;

	curr->plot_line = 1;
	curr->line_type = RA_PLOT_LINE_SOLID;

	curr->line_r = r;
	curr->line_g = g;
	curr->line_b = b;

	strcpy(curr->name, name);
}  // add_pair()


int
raschlab_qt::get_channel(class_handle clh, long ch_type, long **ch, long *num_ch)
{
	*num_ch = 0;
	*ch = NULL;

	meas_handle mh = ra_meas_handle_from_any_handle(clh);
	if (!mh)
		return -1;	
	rec_handle rh = ra_rec_get_first(mh, 0);

	value_handle vh = ra_value_malloc();
	int ret;
	if ((ret = ra_info_get(rh, RA_INFO_REC_GEN_NUM_CHANNEL_L, vh)) != 0)
	{
		ra_value_free(vh);
		return ret;
	}

	long n = ra_value_get_long(vh);
	*ch = new long[n];
	for (long l = 0; l < n; l++)
	{
		ra_value_set_number(vh, l);
		if ((ret = ra_info_get(rh, RA_INFO_REC_CH_TYPE_L, vh)) != 0)
			break;
		if (ra_value_get_long(vh) == ch_type)
		{
			(*ch)[*num_ch] = l;
			(*num_ch)++;
		}
	}

	if ((ret != 0) || (*num_ch <= 0))
	{
		delete[] *ch;
		*ch = NULL;
	}

	ra_value_free(vh);

	return 0;
}  // get_channel()
