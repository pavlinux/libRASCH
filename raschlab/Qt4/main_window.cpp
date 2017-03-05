#include <QtGui/QMainWindow>
#include <QtCore/QSignalMapper>
#include <QtGui/QWorkspace>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QMessageBox>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtCore/QSettings>
#include <libintl.h>

#include <ra_annot_comment_view.h>

#include "main_window.h"

#include "progress_dlg_qt.h"
progress_dlg_qt *gl_progress_dlg;



main_window::main_window(ra_handle h)
{
	if (h)
		ra = h;

	workspace = new QWorkspace;
	setCentralWidget(workspace);
	connect(workspace, SIGNAL(windowActivated(QWidget *)), this, SLOT(update_menus()));
	window_mapper = new QSignalMapper(this);
	connect(window_mapper, SIGNAL(mapped(QWidget *)), workspace, SLOT(setActiveWindow(QWidget *)));

	// at the moment view layouts are set in the code, later they will
	// be either set in a config file or using a wizard or by using
	// the current layout of the views
	init_view_layouts();
	current_layout = 0;
	do_auto_pos = true;

	create_actions();
	create_menus();
	create_toolbars();
	create_statusbar();
	update_menus();

	last_path[0] = '\0';

	settings.read();
	set_recent_used_files();

	QString s = "RASCHlab ";
	s += RASCHLAB_VERSION;
	setWindowTitle(s);

	gl_progress_dlg = new progress_dlg_qt(this);
} // constructor


void
main_window::init_view_layouts()
{
	struct view_layout *vl;
	struct single_view_layout *svl;

	// standard signal/template layout
	vl = new struct view_layout;
	vl->name = "standard";
	vl->desc = "standard measurement layout";

	svl = new struct single_view_layout;
	svl->view_id = SIGNAL_VIEW;
	svl->vert_pos = 0;
	svl->horz_pos = 0;
	svl->height = 1.0;
	svl->width = 4.0/5.0;
	vl->views.push_back(svl);
	svl = new struct single_view_layout;
	svl->view_id = ANNOT_COMMENT_VIEW;
	svl->vert_pos = 0;
	svl->horz_pos = 4.0/5.0;
	svl->height = 1.0;
	svl->width = 1.0/5.0;
	vl->views.push_back(svl);
	
	view_layouts.push_back(vl);

	// standard ECG layout (signal/template/plot)
	vl = new struct view_layout;
	vl->name = "ECG standard";
	vl->desc = "standard ECG layout";

	svl = new struct single_view_layout;
	svl->view_id = SIGNAL_VIEW;
	svl->vert_pos = 0;
	svl->horz_pos = 0;
	svl->height = 2.0/3.0;
	svl->width = 1.0;
	vl->views.push_back(svl);
	svl = new struct single_view_layout;
	svl->view_id = TEMPLATE_VIEW;
	svl->vert_pos = 2.0/3.0;
	svl->horz_pos = 0;
	svl->height = 1.0/3.0;
	svl->width = 1.0/3.0;
	vl->views.push_back(svl);
	svl = new struct single_view_layout;
	svl->view_id = PLOT_VIEW;
	svl->vert_pos = 2.0/3.0;
	svl->horz_pos = 1.0/3.0;
	svl->height = 1.0/3.0;
	svl->width = 2.0/3.0;
	vl->views.push_back(svl);

	view_layouts.push_back(vl);

	// HRT layout (signal/plot/hrt-view)
	// but only if the hrt-view plugin is available
	plugin_handle p = ra_plugin_get_by_name(ra, "hrt-view", 1);
	if (p)
	{
		vl = new struct view_layout;
		vl->name = "HRT";
		vl->desc = "HRT layout";

		svl = new struct single_view_layout;
		svl->view_id = SIGNAL_VIEW;
		svl->vert_pos = 0;
		svl->horz_pos = 0;
		svl->height = 2.0/3.0;
		svl->width = 1.0;
		vl->views.push_back(svl);
		svl = new struct single_view_layout;
		svl->view_id = PLOT_VIEW;
		svl->vert_pos = 2.0/3.0;
		svl->horz_pos = 0;
		svl->height = 1.0/3.0;
		svl->width = 0.5;
		vl->views.push_back(svl);
		svl = new struct single_view_layout;
		svl->view_id = HRT_VIEW;
		svl->vert_pos = 2.0/3.0;
		svl->horz_pos = 0.5;
		svl->height = 1.0/3.0;
		svl->width = 0.5;
		vl->views.push_back(svl);

		view_layouts.push_back(vl);
	}
} // init_view_layouts()


void
main_window::create_actions()
{
        open_act = new QAction(tr("&Open..."), this);
        open_act->setShortcut(tr("Ctrl+O"));
        open_act->setStatusTip(tr("Open a measurement"));
        connect(open_act, SIGNAL(triggered()), this, SLOT(open()));
	
        save_act = new QAction(tr("&Save"), this);
        save_act->setShortcut(tr("Ctrl+S"));
        save_act->setStatusTip(tr("Save evaluation data to disk"));
        connect(save_act, SIGNAL(triggered()), this, SLOT(save()));
	
        exit_act = new QAction(tr("E&xit"), this);
        exit_act->setShortcut(tr("Ctrl+Q"));
        exit_act->setStatusTip(tr("Exit the application"));
        connect(exit_act, SIGNAL(triggered()), this, SLOT(slot_exit()));
	
        close_act = new QAction(tr("Cl&ose"), this);
        close_act->setShortcut(tr("Ctrl+F4"));
        close_act->setStatusTip(tr("Close the active measurement"));
        connect(close_act, SIGNAL(triggered()), this, SLOT(close_active_measurement()));
	
        close_all_act = new QAction(tr("Close &All"), this);
        close_all_act->setStatusTip(tr("Close all the windows"));
        connect(close_all_act, SIGNAL(triggered()), this, SLOT(close_all_measurements()));
	
        tile_act = new QAction(tr("&Tile"), this);
        tile_act->setStatusTip(tr("Tile the windows"));
        connect(tile_act, SIGNAL(triggered()), workspace, SLOT(tile()));
	
        cascade_act = new QAction(tr("&Cascade"), this);
        cascade_act->setStatusTip(tr("Cascade the windows"));
        connect(cascade_act, SIGNAL(triggered()), workspace, SLOT(cascade()));
	
        arrange_act = new QAction(tr("Arrange &icons"), this);
        arrange_act->setStatusTip(tr("Arrange the icons"));
        connect(arrange_act, SIGNAL(triggered()), workspace, SLOT(arrangeIcons()));
	
        next_act = new QAction(tr("Ne&xt"), this);
        next_act->setShortcut(tr("Ctrl+F6"));
        next_act->setStatusTip(tr("Move the focus to the next window"));
        connect(next_act, SIGNAL(triggered()), workspace, SLOT(activateNextWindow()));
	
        previous_act = new QAction(tr("Pre&vious"), this);
        previous_act->setShortcut(tr("Ctrl+Shift+F6"));
        previous_act->setStatusTip(tr("Move the focus to the previous window"));
        connect(previous_act, SIGNAL(triggered()), workspace, SLOT(activatePreviousWindow()));
	
        separator_act = new QAction(this);
        separator_act->setSeparator(true);
	
        about_act = new QAction(tr("&About"), this);
        about_act->setStatusTip(tr("Show the application's About box"));
        connect(about_act, SIGNAL(triggered()), this, SLOT(about()));
	
        about_qt_act = new QAction(tr("About &Qt"), this);
        about_qt_act->setStatusTip(tr("Show the Qt library's About box"));
        connect(about_qt_act, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	create_general_actions();
	create_detect_actions();
} // create_actions()


void
main_window::create_general_actions()
{
	general_plugin_act = new QAction(QString(gettext("&Plugin infos...")), this);
	general_plugin_act->setShortcut(QString(gettext("Ctrl+P")));
	general_plugin_act->setStatusTip(QString(gettext("Show infos about loaded plugins")));
	general_plugin_act->setWhatsThis(QString(gettext("Plugin infos\n\nshow infos about loaded plugins")));
	connect(general_plugin_act, SIGNAL(triggered()), this, SLOT(slot_plugins()));

	general_eval_act = new QAction(QString(gettext("&Evaluation infos...")), this);
	general_eval_act->setShortcut(QString(gettext("Ctrl+E")));
	general_eval_act->setStatusTip(QString(gettext("Show infos about evaluations")));
	general_eval_act->setWhatsThis(QString(gettext("Evaluation infos\n\nshow infos about evaluations")));
	connect(general_eval_act, SIGNAL(triggered()), this, SLOT(slot_eval()));

	save_eval_ascii_act = new QAction(QString(gettext("Save &ASCII eval-values")), this);
	save_eval_ascii_act->setShortcut(QString(gettext("Ctrl+A")));
	save_eval_ascii_act->setStatusTip(QString(gettext("Save the evaluation values as ASCII text")));
	save_eval_ascii_act->setWhatsThis(QString(gettext("Save ASCII eval-values\n\nSave the evaluation values as ASCII text")));
//	save_eval_ascii_act->setChecked(_settings.save_eval_ascii());
	connect(save_eval_ascii_act, SIGNAL(toggled(bool)), this, SLOT(slot_save_eval_ascii(bool)));
} // create_general_actions()


void
main_window::create_detect_actions()
{
	do_detect = new QAction(QString(gettext("&Detect")), this);
	do_detect->setShortcut(QString(gettext("Ctrl+D")));
	do_detect->setStatusTip(QString(gettext("Runs libRASCH beat-detection")));
	do_detect->setWhatsThis(QString(gettext("Detect\n\nRuns libRASCH beat-detection")));
	connect(do_detect, SIGNAL(triggered()), this, SLOT(slot_detect()));

	do_detect_ctg = new QAction(QString(gettext("Detect C&TG")), this);
	do_detect_ctg->setShortcut(QString(gettext("Ctrl+T")));
	do_detect_ctg->setStatusTip(QString(gettext("Runs libRASCH CTG-detection")));
	do_detect_ctg->setWhatsThis(QString(gettext("Detect\n\nRuns libRASCH CTG-detection")));
	connect(do_detect_ctg, SIGNAL(triggered()), this, SLOT(slot_detect_ctg()));

	do_rr_morph = new QAction(QString(gettext("&RR-Morphology")), this);
	do_rr_morph->setShortcut(QString(gettext("Ctrl+R")));
	do_rr_morph->setStatusTip(QString(gettext("Runs libRASCH RR-Morphology Plugin")));
	do_rr_morph->setWhatsThis(QString(gettext("RR-Morphology\n\nRuns libRASCH RR-Morphology Plugin")));
	connect(do_rr_morph, SIGNAL(triggered()), this, SLOT(slot_rr_morphology()));

	do_ecg_class = new QAction(QString(gettext("&Classify ECG")), this);
	do_ecg_class->setShortcut(QString(gettext("Ctrl+C")));
	do_ecg_class->setStatusTip(QString(gettext("Runs libRASCH ecg Plugin")));
	do_ecg_class->setWhatsThis(QString(gettext("Classify ECG\n\nRuns libRASCH ecg Plugin")));
	connect(do_ecg_class, SIGNAL(triggered()), this, SLOT(slot_classify_ecg()));

	do_calc = new QAction(QString(gettext("&Perform Calculations")), this);
	do_calc->setShortcut(QString(gettext("Ctrl+P")));
	do_calc->setStatusTip(QString(gettext("Perform Calculations using libRASCH process plugins")));
	do_calc->setWhatsThis(QString(gettext("Perform Calculations\n\n"
							"Perform Calculations using libRASCH process plugins")));
	connect(do_calc, SIGNAL(triggered()), this, SLOT(slot_calc()));
} // create_detect_actions()


void
main_window::create_menus()
{
	file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(open_act);
	file_menu->addAction(save_act);
	file_menu->addSeparator();
	file_menu->addAction(close_act);
	file_menu->addAction(close_all_act);
	file_menu->addSeparator();
	recent_files_menu = file_menu->addMenu(tr("Recent Files"));
	file_menu->addSeparator();
	file_menu->addAction(exit_act);
	
	general_menu = menuBar()->addMenu(tr("&General"));
	general_menu->addAction(general_plugin_act);
	general_menu->addAction(general_eval_act);
//	general_menu->addAction(save_eval_ascii_act);

	detect_menu = menuBar()->addMenu(tr("&Detect"));

	create_view_menu();

	window_menu = menuBar()->addMenu(tr("&Window"));
	connect(window_menu, SIGNAL(aboutToShow()), this, SLOT(update_window_menu()));
	
	menuBar()->addSeparator();
	
	help_menu = menuBar()->addMenu(tr("&Help"));
	help_menu->addAction(about_act);
	help_menu->addAction(about_qt_act);
} // create_menus()


void
main_window::create_view_menu()
{
	auto_pos_act = new QAction(QString(gettext("&Auto-Pos meas-views")), this);
	//auto_pos_act->setShortcut(QString(gettext("Ctrl+A")));
	auto_pos_act->setStatusTip(QString(gettext("Position the views of a measurement automatically.")));
	auto_pos_act->setWhatsThis(QString(gettext("Auto-Pos meas-views\n\nPosition the views of a measurement automatically.")));
	connect(auto_pos_act, SIGNAL(toggled(bool)), this, SLOT(slot_auto_pos(bool)));
	auto_pos_act->setCheckable(true);
	auto_pos_act->setChecked(do_auto_pos);

	view_menu = menuBar()->addMenu(tr("&View"));
	view_menu->addAction(auto_pos_act);
	view_menu->addSeparator()->setText(tr("Layouts"));;

	layout_act_group = new QActionGroup(this);
	layout_mapper = new QSignalMapper(this);
	for (size_t i = 0; i < view_layouts.size(); i++)
	{
 		QAction *action = view_menu->addAction(view_layouts[i]->name);
		action->setStatusTip(view_layouts[i]->desc);
		action->setCheckable(true);
		
		connect(action, SIGNAL(triggered()), layout_mapper, SLOT(map()));
		layout_mapper->setMapping(action, i);

		layout_act_group->addAction(action);

		if (i == (size_t)current_layout)
			action->setChecked(true);
		else
			action->setChecked(false);
	}

	connect(layout_mapper, SIGNAL(mapped(int)), this, SLOT(slot_layout(int)));
} // create_view_menu()


void
main_window::update_detect_menu(signal_ra_qt *sig)
{
	// first remove all menu entries and add do-calc entry which is always available
	detect_menu->clear();
	detect_menu->addAction(do_calc);
	
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
				detect_menu->addAction(do_detect);
				detect_menu->addAction(do_ecg_class);
				ecg = true;
			}
			break;
		case RA_CH_TYPE_RR:
			if (!rr)
			{
				detect_menu->addAction(do_rr_morph);
				rr = true;
			}
			break;
		case RA_CH_TYPE_CTG_UC:
			if (!ctg)
			{
				detect_menu->addAction(do_detect_ctg);
				ctg = true;
			}
			break;
		}
	}
	ra_value_free(vh);
} // update_detect_menu()


void
main_window::create_toolbars()
{
        file_toolbar = addToolBar(tr("File"));
        file_toolbar->addAction(open_act);
        file_toolbar->addAction(save_act);
	
        edit_toolbar = addToolBar(tr("Edit"));
} // create_toolbars()


void
main_window::create_statusbar()
{
        statusBar()->showMessage(tr("Ready"));
} // create_statusbar()


void
main_window::update_menus()
{
        bool has_mdi_child = (active_signal() != NULL);
        save_act->setEnabled(has_mdi_child);
        close_act->setEnabled(has_mdi_child);
        close_all_act->setEnabled(has_mdi_child);
        tile_act->setEnabled(has_mdi_child);
        cascade_act->setEnabled(has_mdi_child);
        arrange_act->setEnabled(has_mdi_child);
        next_act->setEnabled(has_mdi_child);
        previous_act->setEnabled(has_mdi_child);
        separator_act->setVisible(has_mdi_child);
} // update_menus()


void
main_window::update_window_menu()
{
        window_menu->clear();
        window_menu->addAction(close_act);
        window_menu->addAction(close_all_act);
        window_menu->addSeparator();
        window_menu->addAction(tile_act);
        window_menu->addAction(cascade_act);
        window_menu->addAction(arrange_act);
        window_menu->addSeparator();
        window_menu->addAction(next_act);
        window_menu->addAction(previous_act);
        window_menu->addAction(separator_act);
/*
        QList<QWidget *> windows = workspace->windowList();
        separator_act->setVisible(!windows.isEmpty());
	
	signal_ra_qt *curr_sig = active_signal();

        for (int i = 0; i < windows.size(); ++i)
	{
		mdi_child *child = qobject_cast<mdi_child *>(windows.at(i));
		
		QString text;
		if (i < 9)
		{
			text = tr("&%1 %2").arg(i + 1) .arg(child->user_friendly_current_file());
		}
		else
		{
			text = tr("%1 %2").arg(i + 1) .arg(child->user_friendly_current_file());
		}
		QAction *action  = window_menu->addAction(text);
		action->setCheckable(true);
		
		if (curr_sig)
		{
			if (curr_sig->find_view(child))
				action->setChecked(true);
			else
				action->setChecked(false);
		}

		connect(action, SIGNAL(triggered()), window_mapper, SLOT(map()));
		window_mapper->setMapping(action, child);
        }
*/
} // update_window_menu()


void
main_window::set_recent_used_files()
{
	recent_files_menu->clear();
	recent_used_files.clear();

	recent_files_mapper = new QSignalMapper(this);
	recent_used_files = settings.get_recent_used_files();
	for (size_t i = 0; i < recent_used_files.size(); i++)
	{
		QAction *act = recent_files_menu->addAction(recent_used_files[i].c_str());
		connect(act, SIGNAL(triggered()), recent_files_mapper, SLOT(map()));
		recent_files_mapper->setMapping(act, i);
	}

	connect(recent_files_mapper, SIGNAL(mapped(int)), this, SLOT(slot_recent_files(int)));
} // set_recent_used_files()


signal_ra_qt *
main_window::active_signal()
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
main_window::closeEvent(QCloseEvent *event)
{
	while (!signal_list.isEmpty())
	{
		delete signal_list.takeFirst();
	}

	settings.save();

	QMainWindow::closeEvent(event);
} // close_event()


void
main_window::open()
{
	QString fn = select_signal();
	if (fn.isEmpty())
		return;

	open_signal_file(fn);

	statusBar()->showMessage(QString(gettext("Measurement loaded")), 2000);
} // open()


QString
main_window::select_signal()
{
	QString sig = "";
	
	plugin_handle plugin = ra_plugin_get_by_name(ra, "sig-sel-dlg", 1);
	if (!plugin)
		return sig;
		
	proc_handle proc = ra_proc_get(NULL, plugin, NULL);
	value_handle vh = ra_value_malloc();
	ra_value_set_string(vh, (const char *)last_path);
	ra_lib_set_option(proc, "initial_path", vh);

	ra_gui_call(proc, plugin);

	if (ra_proc_get_result(proc, 0, 0, vh) == 0)
		sig = ra_value_get_string(vh);
	ra_value_free(vh);

	ra_proc_free(proc);

	strcpy(last_path, sig.toLocal8Bit().data());
	char *p = strrchr(last_path, '/');
	if (p)
		*p = '\0';

	return sig;	
} // select_signal()


void
main_window::open_signal_file(QString file)
{
	if (file.isEmpty())
		return;

	QApplication::setOverrideCursor(Qt::WaitCursor);

	signal_ra_qt *sig = new signal_ra_qt(ra);
	if (!sig->open(file.toLocal8Bit().constData()))
	{
		QApplication::restoreOverrideCursor();
		delete sig;
		return;
	}
	signal_list.append(sig);
	if (create_view(sig) != 0)
	{
		QApplication::restoreOverrideCursor();
		return;
	}

	settings.add_recent_used_file(file.toLocal8Bit().constData());
	settings.save();  // save settings to store added file now (in case RASCHlab crashes,
	                  // the recently opened file is not stored in the list)
	set_recent_used_files();
	
	update_detect_menu(sig);

	sig->load_default_settings();

	QApplication::restoreOverrideCursor();
} // open_signal_file()


int
main_window::create_view(signal_ra_qt *sig)
{
	// FIXME: type of view will be read from some config-files or similar
	plugin_handle p = ra_plugin_get_by_name(ra, "rasch-view", 1);
	if (!p)
		return -1;

	char title[100];
	sig->get_title(title, 100);

	view_handle vih = ra_view_get(sig->get_meas_handle(), p, this);
	if (!vih)
		return -1;
	if (ra_view_create(vih) != 0)
	{
		ra_view_free(vih);
		return -1;
	}

 	QWidget *rec_view = (QWidget *)ra_view_get_window(vih, 0);
	if (!rec_view)
	{
		ra_view_free(vih);
		return -1;
	}
        workspace->addWindow(rec_view);

	QString t = QString(gettext("recording: "));
	t += title;
	rec_view->setWindowTitle(t);
	sig->add_view(rec_view, vih, SIGNAL_VIEW);
	
	eval_handle eh = ra_eval_get_default(sig->get_meas_handle());
	if (!eh)
	{
		rec_view->showMaximized();
		return 0;
	}

	// decide if template window is shown (and with which infos)
	QWidget *templ_view = NULL;
	QWidget *hrt_view = NULL;
	value_handle vh = ra_value_malloc();
	ra_class_get(eh, "heartbeat", vh);
	if (ra_value_get_num_elem(vh) > 0)
	{
		class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
		ra_sum_get(clh, "template", vh);
		if (ra_value_get_num_elem(vh) > 0)
			templ_view = show_qrs_templates(sig, (sum_handle)(ra_value_get_voidp_array(vh)[0]));
		if (templ_view)
			workspace->addWindow(templ_view);

		hrt_view = show_hrt_view(sig);
		if (hrt_view)
			workspace->addWindow(hrt_view);
	}

	ra_class_get(eh, "uterine-contraction", vh);
	if (ra_value_get_num_elem(vh) > 0)
	{
		class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
		ra_sum_get(clh, "template", vh);
		if (ra_value_get_num_elem(vh) > 0)
			templ_view = show_uc_templates(sig, (sum_handle)(ra_value_get_voidp_array(vh)[0]));
		if (templ_view)
			workspace->addWindow(templ_view);
	}
	ra_value_free(vh);

	// create annot-comment view
	QWidget *annot_view = NULL;
	p = ra_plugin_get_by_name(ra, "annot-comment-view", 1);
	if (p)
	{
		struct ra_annot_comment_view *opt = (struct ra_annot_comment_view *)ra_alloc_mem(sizeof(struct ra_annot_comment_view));
		memset(opt, 0, sizeof(struct ra_annot_comment_view));
		opt->eh = eh;

		vih = ra_view_get(sig->get_meas_handle(), p, workspace);

		struct view_info *vi = (struct view_info *)vih;
		vi->options = opt;
		ra_view_create(vih);
		annot_view = (QWidget *)ra_view_get_window(vih, 0);
		t = "Annotations/Comments: ";
		t += title;
		annot_view->setWindowTitle(t);
		sig->add_view(annot_view, vih, ANNOT_COMMENT_VIEW);
		workspace->addWindow(annot_view);
	}

	// create plot view
	QWidget *plot_view= NULL;
	p = ra_plugin_get_by_name(ra, "plot-view", 1);
	if (p)
	{
		struct ra_plot_options *opt = (struct ra_plot_options *)ra_alloc_mem(sizeof(struct ra_plot_options));
		memset(opt, 0, sizeof(struct ra_plot_options));
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
			workspace->addWindow(plot_view);
		}
		else
		{
			if (opt->clh)
			{
				ra_free_mem(opt->clh);
				if (opt->class_name)
				{
					for (int i = 0; i < opt->num_event_class; i++)
						ra_free_mem(opt->class_name[i]);
					ra_free_mem(opt->class_name);
				}
			}
			ra_free_mem(opt);
		}
	}
	
	if (!plot_view && !templ_view && !hrt_view && !annot_view)
		rec_view->showMaximized();
	else
	{
		rec_view->show();
		if (plot_view)
			plot_view->show();
		if (templ_view)
			templ_view->show();
		if (hrt_view)
			hrt_view->show();
		if (annot_view)
			annot_view->show();
	}

	resizeEvent(NULL);

	return 0;
} // create_view()


QWidget *
main_window::show_qrs_templates(signal_ra_qt *sig, sum_handle sh)
{
 	plugin_handle p = ra_plugin_get_by_name(ra, "ev-summary-view", 1);
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
 	QString t = QString(gettext("qrs-templates: "));
 	char title[100];
 	sig->get_title(title, 100);
 	t += title;
 	view->setWindowTitle(t);
 	sig->add_view(view, vih, TEMPLATE_VIEW);
	
 	return view;
}  // show_qrs_templates()


QWidget *
main_window::show_hrt_view(signal_ra_qt *sig)
{
 	plugin_handle p = ra_plugin_get_by_name(ra, "hrt-view", 1);
 	if (!p)
 		return NULL;
	
	view_handle vih = ra_view_get(sig->get_meas_handle(), p, workspace);
	if (vih == NULL)
		return NULL;

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
 	QString t = QString(gettext("HRT tachogram: "));
 	char title[100];
 	sig->get_title(title, 100);
 	t += title;
 	view->setWindowTitle(t);
 	sig->add_view(view, vih, HRT_VIEW);
	
 	return view;
}  // show_hrt_view()


QWidget *
main_window::show_uc_templates(signal_ra_qt *sig, sum_handle sh)
{
 	plugin_handle p = ra_plugin_get_by_name(ra, "ev-summary-view", 1);
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
 	QString t = QString(gettext("uc-templates: "));
 	char title[100];
 	sig->get_title(title, 100);
 	t += title;
 	view->setWindowTitle(t);
 	sig->add_view(view, vih, TEMPLATE_VIEW);
	
 	return view;
}  // show_uc_templates()


void
main_window::save()
{
	signal_ra_qt *curr = active_signal();
	if (!curr)
		return;

        curr->save(0);
	statusBar()->showMessage(tr("Measurement saved"), 2000);
} // save()


void
main_window::about()
{
	value_handle vh = ra_value_malloc();
	ra_info_get(ra, RA_INFO_VERSION_C, vh);
	QString s = "RASCHlab Version ";
	s += RASCHLAB_VERSION;
	s += "\n(c) 2002-2009 by Raphael Schneider (librasch@gmail.com)\nusing libRASCH Version ";
	s += ra_value_get_string(vh);
	ra_value_free(vh);
	QMessageBox::about(this, QString(gettext("About RASCHlab")), s);
} // about()


void
main_window::close_active_measurement()
{
	signal_ra_qt *curr = active_signal();
	if (!curr)
		return;

	int idx = signal_list.indexOf(curr);
	delete curr;
	signal_list.removeAt(idx);

	statusBar()->showMessage(tr("Measurement closed"), 2000);
} // close_active_measurement()


void
main_window::close_all_measurements()
{
	while (!signal_list.isEmpty())
	{
		signal_ra_qt *curr = signal_list.takeFirst();
		int idx = signal_list.indexOf(curr);
		signal_list.removeAt(idx);
		delete curr;

	}
} // close_all_measurements()


void
main_window::slot_plugins()
{
	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, "plugin-info-dlg", 1)) == NULL)
		return;

	proc_handle proc = ra_proc_get(NULL, pl, NULL);
	ra_gui_call(proc, pl);
	ra_proc_free(proc);
}  // slot_plugins()


void
main_window::slot_eval()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;
	
	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, "eval-dlg", 1)) == NULL)
		return;

	proc_handle proc = ra_proc_get(mh, pl, NULL);
	if (ra_gui_call(proc, pl) == 1) // if 'OK', save changes
	{
 		ra_eval_save(mh, NULL, 0); //(int)_settings.save_eval_ascii());

		char fn[MAX_PATH_RA];
		sig->filename(fn, MAX_PATH_RA);

		sig->close();
		int idx = signal_list.indexOf(sig);
		signal_list.removeAt(idx);
		delete sig;
		
		open_signal_file(fn);
	}
	ra_proc_free(proc);
}  // slot_eval()


void
main_window::slot_save_eval_ascii(bool /*toggle*/)
{
	statusBar()->showMessage(QString(gettext("Toggle that eval-values are saved in ASCII...")));

//	_settings.set_save_eval_ascii(toggle);

	statusBar()->showMessage(QString(gettext("Ready.")));
}  // slot_save_eval_ascii()


void
detect_callback(const char *text, int percent)
{
	if (text)
		gl_progress_dlg->set_text(text);
	gl_progress_dlg->set_progress(percent);
	
	qApp->processEvents();
} // detect_callback()


void
main_window::slot_detect()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	int num_ch = -1;
	long *ch = NULL;

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, "ch-select-dlg", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();
		long ch_type = RA_CH_TYPE_ECG;
		ra_value_set_long_array(vh, &ch_type, 1);
		ra_lib_set_option(proc, "ch_type_filter", vh);
		
		int ret = ra_gui_call(proc, pl);
		if (ret != 0)
		{
			ra_proc_free(proc);
			ra_value_free(vh);
			return;
		}

		ra_proc_get_result(proc, 0, 0, vh);
		num_ch = ra_value_get_num_elem(vh);
		ch = new long[num_ch];
		const long *lp = ra_value_get_long_array(vh);
		for (long l = 0; l < num_ch; l++)
			ch[l] = lp[l];
		ra_value_free(vh);

		ra_proc_free(proc);
	}

	gl_progress_dlg->show();	
	qApp->processEvents();

	/* prepare eval to store the detection results */
	eval_handle eh_save = ra_eval_get_default(mh);
	eval_handle eh = ra_eval_add(mh, "RASCHlab", "evaluation created within RASCHlab", 0);
	if (!eh)
	{
		gl_progress_dlg->hide();
		delete[] ch;
		return;
	}
	
	if ((pl = ra_plugin_get_by_name(ra, "detect-ecg", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long_array(vh, ch, num_ch);
		ra_lib_set_option(proc, "ch", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_eval", vh);
		ra_value_set_voidp(vh, eh);
		ra_lib_set_option(proc, "eh", vh);
 		ra_value_set_short(vh, 1);
 		ra_lib_set_option(proc, "do_interpolation", vh);

		ra_value_free(vh);

		ra_proc_do(proc);
		ra_proc_free(proc);
	}

	value_handle vh = ra_value_malloc();
	if ((ra_class_get(eh, "heartbeat", vh) != 0) ||
		(ra_value_get_num_elem(vh) <= 0))
	{
		gl_progress_dlg->hide();
		delete[] ch;
		ra_eval_delete(eh);
		if (eh_save)
			ra_eval_set_default(eh_save);
		return;
	}
	const void **temp = ra_value_get_voidp_array(vh);
	if (temp == NULL)
		return;
	class_handle clh = (class_handle)(temp[0]);
	ra_value_free(vh);

	if ((pl = ra_plugin_get_by_name(ra, "fiducial-point", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(proc, "clh", vh);
 		ra_value_set_string(vh, "qrs-pos");
 		ra_lib_set_option(proc, "pos_prop", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_class", vh);
		ra_value_set_long(vh, ch[0]);
		ra_lib_set_option(proc, "ch", vh);
		ra_value_set_double(vh, 0.03);
		ra_lib_set_option(proc, "corr_len", vh);

		ra_value_free(vh);

		ra_proc_do(proc);
		ra_proc_free(proc);
	}

 	if ((pl = ra_plugin_get_by_name(ra, "template", 1)) != NULL)
 	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

 		value_handle vh = ra_value_malloc();

 		ra_value_set_short(vh, 1);
 		ra_lib_set_option(proc, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(proc, "clh", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_class", vh);
 		ra_value_set_string(vh, "qrs-pos");
 		ra_lib_set_option(proc, "pos_prop", vh);
 		ra_value_set_string(vh, "qrs-template");
 		ra_lib_set_option(proc, "templ_name", vh);
 		ra_value_set_string(vh, "qrs-template-corr");
 		ra_lib_set_option(proc, "templ_corr", vh);
//		ra_value_set_double(vh, 0.05);
		ra_value_set_double(vh, 0.10);
 		ra_lib_set_option(proc, "corr_win_before", vh);
//		ra_value_set_double(vh, 0.15);
		ra_value_set_double(vh, 0.10);
 		ra_lib_set_option(proc, "corr_win_after", vh);
 		ra_value_set_double(vh, 0.85);
 		ra_lib_set_option(proc, "accept", vh);
// 		ra_value_set_double(vh, 0.6);
 		ra_value_set_double(vh, 0.7);
 		ra_lib_set_option(proc, "slope_accept_low", vh);
// 		ra_value_set_double(vh, 1.4);
 		ra_value_set_double(vh, 1.3);
 		ra_lib_set_option(proc, "slope_accept_high", vh);
 		ra_value_set_double(vh, 0.20);
 		ra_lib_set_option(proc, "template_win_before", vh);
 		ra_lib_set_option(proc, "template_win_after", vh);
 		ra_value_set_long_array(vh, ch, num_ch);
 		ra_lib_set_option(proc, "ch", vh);
 		ra_value_set_short(vh, 1);
 		ra_lib_set_option(proc, "align_events", vh);

 		ra_value_free(vh);

 		ra_proc_do(proc);
 		ra_proc_free(proc);
 	}

 	if ((pl = ra_plugin_get_by_name(ra, "ap-morphology", 1)) != NULL)
 	{
 		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(proc, "clh", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_class", vh);
 		ra_value_set_short(vh, 0);
 		ra_lib_set_option(proc, "rr_pos", vh);
 		ra_lib_set_option(proc, "dont_search_calibration", vh);

		ra_value_free(vh);

 		ra_proc_do(proc);
 		ra_proc_free(proc);
 	}

	if ((pl = ra_plugin_get_by_name(ra, "respiration", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "use_eval", vh);
		ra_value_set_voidp(vh, eh);
		ra_lib_set_option(proc, "eh", vh);
		ra_value_set_long(vh, -1);
		ra_lib_set_option(proc, "ch_num", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_eval", vh);

		ra_value_free(vh);

		ra_proc_do(proc);
		ra_proc_free(proc);
	}

 	if ((pl = ra_plugin_get_by_name(ra, "ecg", 1)) != NULL)
 	{
 		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(proc, "clh", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_class", vh);

		ra_value_free(vh);

 		ra_proc_do(proc);
 		ra_proc_free(proc);
 	}

	detect_callback("save evaluation file", 0);	
 	ra_eval_save(mh, NULL, 0); //(int)_settings.save_eval_ascii());
		
	gl_progress_dlg->hide();
	delete[] ch;

	char fn[MAX_PATH_RA];
	sig->filename(fn, MAX_PATH_RA);

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;

	open_signal_file(fn);
}  // slot_detect()


void
main_window::slot_detect_ctg()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	long num_ch = -1;
	long *ch = NULL;

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, "ch-select-dlg", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();
		long ch_type = RA_CH_TYPE_CTG_UC;
		ra_value_set_long_array(vh, &ch_type, 1);
		ra_lib_set_option(proc, "ch_type_filter", vh);

		int ret = ra_gui_call(proc, pl);
		if (ret != 0)
		{
			ra_value_free(vh);
			ra_proc_free(proc);
			return;
		}

		ra_proc_get_result(proc, 0, 0, vh);
		num_ch = ra_value_get_num_elem(vh);
		ch = new long[num_ch];
		const long *lp = ra_value_get_long_array(vh);
		for (long l = 0; l < num_ch; l++)
			ch[l] = lp[l];
		ra_value_free(vh);

		ra_proc_free(proc);
	}

		
	gl_progress_dlg->show();
 	qApp->processEvents();
	
	/* prepare eval to store the detection results */
	eval_handle eh_save = ra_eval_get_default(mh);
	eval_handle eh = ra_eval_add(mh, "RASCHlab", "evaluation created within RASCHlab", 0);
	if (!eh)
	{
		gl_progress_dlg->hide();
		delete[] ch;
		return;
	}
	
	if ((pl = ra_plugin_get_by_name(ra, "detect-ctg", 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		value_handle vh = ra_value_malloc();

		ra_value_set_long_array(vh, ch, num_ch);
		ra_lib_set_option(proc, "ch", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_eval", vh);
		ra_value_set_voidp(vh, eh);
		ra_lib_set_option(proc, "eh", vh);

		ra_value_free(vh);

		ra_proc_do(proc);
		ra_proc_free(proc);
	}

	value_handle vh = ra_value_malloc();
	if ((ra_class_get(eh, "uterine-contraction", vh) != 0) ||
		(ra_value_get_num_elem(vh) <= 0))
	{
		gl_progress_dlg->hide();
		delete[] ch;
		ra_eval_delete(eh);
		if (eh_save)
			ra_eval_set_default(eh_save);
		return;
	}
	class_handle clh = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	ra_value_free(vh);

 	if ((pl = ra_plugin_get_by_name(ra, "template", 1)) != NULL)
 	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

 		value_handle vh = ra_value_malloc();

 		ra_value_set_short(vh, 1);
 		ra_lib_set_option(proc, "use_class", vh);
		ra_value_set_voidp(vh, clh);
		ra_lib_set_option(proc, "clh", vh);
		ra_value_set_short(vh, 1);
		ra_lib_set_option(proc, "save_in_class", vh);
		ra_value_set_string(vh, "uc-max-pos");
		ra_lib_set_option(proc, "pos_name", vh);
		ra_value_set_string(vh, "uc-template");
		ra_lib_set_option(proc, "templ_name", vh);
		ra_value_set_string(vh, "uc-template-corr");
		ra_lib_set_option(proc, "templ_corr", vh);
		ra_value_set_double(vh, 0.85);
		ra_lib_set_option(proc, "accept", vh);
		ra_value_set_double(vh, 0.6);
		ra_lib_set_option(proc, "slope_accept_low", vh);
		ra_value_set_double(vh, 1.4);
		ra_lib_set_option(proc, "slope_accept_high", vh);
		ra_value_set_double(vh, 40);
		ra_lib_set_option(proc, "win_before", vh);
		ra_value_set_double(vh, 40);
		ra_lib_set_option(proc, "win_after", vh);
		ra_value_set_long_array(vh, ch, num_ch);
		ra_lib_set_option(proc, "ch", vh);

 		ra_value_free(vh);

 		ra_proc_do(proc);
 		ra_proc_free(proc);
 	}

	detect_callback("save evaluation file", 0);	
 	ra_eval_save(mh, NULL, 0); //(int)_settings.save_eval_ascii());
		
	gl_progress_dlg->hide();
	delete[] ch;

	char fn[MAX_PATH_RA];
	sig->filename(fn, MAX_PATH_RA);

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;

	open_signal_file(fn);
}  // slot_detect_ctg()


void
main_window::slot_rr_morphology()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	char fn[MAX_PATH_RA];
	sig->filename(fn, MAX_PATH_RA);

	process_signal("ap-morphology", true);

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;
	open_signal_file(fn);
}  // slot_rr_morphology()


void
main_window::process_signal(const char *plugin_name, bool save_eval/* = false */)
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

	gl_progress_dlg->show();
	qApp->processEvents();

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, plugin_name, 1)) != NULL)
	{
		proc_handle proc = ra_proc_get(mh, pl, detect_callback);

		ra_proc_do(proc);
		ra_proc_free(proc);
	}

	if (save_eval)
	{
		detect_callback("save evaluation file", 0);	
 		ra_eval_save(mh, NULL, 0); //(int)_settings.save_eval_ascii());
	}

	gl_progress_dlg->hide();
}  // process_signal()


void
main_window::slot_classify_ecg()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	char fn[MAX_PATH_RA];
	sig->filename(fn, MAX_PATH_RA);

	process_signal("ecg");

	sig->close();
	int idx = signal_list.indexOf(sig);
	signal_list.removeAt(idx);
	delete sig;
	open_signal_file(fn);
}  // slot_classify_ecg()


void
main_window::slot_calc()
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(ra, "simple-calc-dlg", 1)) == NULL)
		return;

	proc_handle proc = ra_proc_get(mh, pl, NULL);
	ra_gui_call(proc, pl);
	ra_proc_free(proc);
}  // slot_calc()


// old version of resizeEvent function (keep to have code available
// to see how it worked before)
// void
// main_window::resizeEvent(QResizeEvent *)
// {
// 	signal_ra_qt *sig = active_signal();
// 	if (!sig)
// 		return;

// 	int n = sig->num_views();	

// 	if (n <= 1)
// 		return;  // is alreay maximized

// 	bool no_template_view = true;
// 	bool no_hrt_view = true;
// 	QWidget *v = NULL;
// 	for (int i = 0; i < n; i++)
// 	{
// 		v = (QWidget *)sig->get_view(v);
// 		if (!v)
// 			break;
		
// 		if (sig->get_view_type(v) == TEMPLATE_VIEW)
// 		{
// 			no_template_view = false;
// 			break;
// 		}

// 		if (sig->get_view_type(v) == HRT_VIEW)
// 		{
// 			no_hrt_view = false;
// 			break;
// 		}
// 	}

// 	QRect r = workspace->geometry();
//  	int pv_start, pv_height;
//  	if (sig->num_plot_views() > 0)
//  	{
//  		pv_start = (int)((double)(r.height()) * (sig->num_plot_views() >= 2 ? 0.33 : 0.66));
//  		pv_height = (r.height() - pv_start) / sig->num_plot_views();
//  	}

// 	v = NULL;
// 	int num_plot_view_shown = 0;
// 	for (int i = 0; i < n; i++)
// 	{
// 		v = (QWidget *)sig->get_view(v);
// 		if (!v)
// 			break;
		
// 		int type = sig->get_view_type(v);
// 		int x, y, w, h;
// 		bool dont_move = false;
// 		switch (type)
// 		{
// 		case SIGNAL_VIEW:
// 			x = y = 0;
// 			w = r.width();
// 			if (sig->num_plot_views() >= 2)
// 				h = (int)((double)(r.height()) * 0.33);
// 			else
// 				h = (int)((double)(r.height()) * 0.66);
// 			break;
// 		case TEMPLATE_VIEW:
// 			x = 0;
// 			y = (int)((double)(r.height()) * 0.66);
// 			if (sig->num_plot_views() == 0)
// 				w = r.width();
// 			else
// 				w = (int)((double)(r.width()) * 0.33);
// 			h = r.height() - y;
// 			break;
// 		case HRT_VIEW:
// 			x = 0;
// 			y = (int)((double)(r.height()) * 0.66);
// 			if (sig->num_plot_views() == 0)
// 				w = r.width();
// 			else
// 				w = (int)((double)(r.width()) * 0.33);
// 			h = r.height() - y;
// 			break;
// 		case PLOT_VIEW:
// 			if (no_template_view && no_hrt_view)
// 				x = 0;
// 			else
// 				x = (int)((double)(r.width()) * 0.33);
// 			w = r.width() - x;
// 			y = pv_start + (num_plot_view_shown * pv_height);
// 			h = pv_height;
// 			num_plot_view_shown++;
// 			break;
// 		default:
// 			dont_move = true;
// 			break;
// 		}
// 		if (dont_move)
// 			continue;
			
// 		v->parentWidget()->setGeometry(x, y, w, h);
// 	}
// 	sig->update_all_views(NULL);
// }  // slot_resize()


void
main_window::resizeEvent(QResizeEvent *)
{
	signal_ra_qt *sig = active_signal();
	if (!sig)
		return;

	int n = sig->num_views();	

	if (n <= 1)
		return;  // is alreay maximized

	if (!do_auto_pos)
		return;  // auto-pos is disabled

	// get available space
	QRect r = workspace->geometry();

	// check for each available view if it has to be shown,
	// at which position and with which size
	struct view_layout *curr = view_layouts[current_layout];
	QWidget *v = NULL;
	for (int i = 0; i < n; i++)
	{
		v = (QWidget *)sig->get_view(v);
		if (!v)
			break;
		
		int type = sig->get_view_type(v);

		int idx = -1;
		for (size_t j = 0; j < curr->views.size(); j++)
		{
			if (curr->views[j]->view_id == type)
			{
				idx = j;
				break;
			}
		}
		if (idx == -1)
		{
			v->hide();
			continue;
		}

		v->show();

		int x = (int)(curr->views[idx]->horz_pos * (double)r.width());
		int y = (int)(curr->views[idx]->vert_pos * (double)r.height());
		int w = (int)(curr->views[idx]->width * (double)r.width());
		int h = (int)(curr->views[idx]->height * (double)r.height());

		v->parentWidget()->setGeometry(x, y, w, h);
	}
	sig->update_all_views(NULL);
}  // slot_resize()



bool
main_window::fill_plot_infos(signal_ra_qt *sig, struct ra_plot_options *opt)
{
	if (opt->clh)
	{
		ra_free_mem(opt->clh);
		opt->clh = NULL;
	}
	if (opt->class_name)
	{
		for (int i = 0; i < opt->num_event_class; i++)
			ra_free_mem(opt->class_name[i]);
		ra_free_mem(opt->class_name);

		opt->class_name = NULL;
	}	

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
	opt->num_event_class = 1;
	opt->clh = (class_handle *)ra_alloc_mem(sizeof(class_handle) * opt->num_event_class);
	opt->clh[0] = (class_handle)(ra_value_get_voidp_array(vh)[0]);
	opt->class_name = (char **)ra_alloc_mem(sizeof(char *) * opt->num_event_class);
	opt->class_name[0] = (char *)ra_alloc_mem(sizeof(char) * EVAL_MAX_NAME);
 	strcpy(opt->class_name[0], "heartbeat");

	int curr_clh_idx = 0;	
 	prop_handle pos = ra_prop_get(opt->clh[curr_clh_idx], "qrs-pos");
  	prop_handle rri = ra_prop_get(opt->clh[curr_clh_idx], "rri");
   	prop_handle syst = ra_prop_get(opt->clh[curr_clh_idx], "rr-systolic");
    	prop_handle dias = ra_prop_get(opt->clh[curr_clh_idx], "rr-diastolic");
    	prop_handle resp = ra_prop_get(opt->clh[curr_clh_idx], "resp-chest-mean-rri");

 	opt->show_legend = 1;

 	if (!pos)
 		return false;

 	add_pos_axis(opt, curr_clh_idx, pos);
 	bool some_added = false;
 	if (rri)
 	{
 		add_rri_axis(opt, curr_clh_idx, rri);
 		add_pair(opt, 0, (opt->num_axis-1), "RR-interval", 0, 0, 0);
 		some_added = true;
 	}
 	if (syst)
 	{
 		add_syst_axis(opt, curr_clh_idx, syst);
 		add_pair(opt, 0, (opt->num_axis-1), "Systolic", 255, 0, 0);
 		some_added = true;
 	}
 	if (dias)
 	{
 		add_dias_axis(opt, curr_clh_idx, dias);
 		add_pair(opt, 0, (opt->num_axis-1), "Diastolic", 0, 0, 255);
 		some_added = true;
 	}
 	if (resp)
 	{
 		add_resp_axis(opt, curr_clh_idx, resp);
 		add_pair(opt, 0, (opt->num_axis-1), "Respiration", 0, 255, 0);
 		some_added = true;
 	}

 	return some_added;
}  // fill_plot_infos()


struct ra_plot_axis *
main_window::add_axis(struct ra_plot_options *opt)
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
main_window::add_pos_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	curr->event_class_idx = curr_clh_idx;
	strcpy(curr->prop_name, "qrs-pos");
	curr->event_property = prop;
	strcpy(curr->unit, "min");
	curr->is_time = 1;
	curr->time_format = RA_PLOT_TIME_MIN;
	curr->ch = -1;
}  // add_pos_axis()


void
main_window::add_rri_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	curr->event_class_idx = curr_clh_idx;
	strcpy(curr->prop_name, "rri");
	curr->event_property = prop;
	strcpy(curr->unit, "ms");
	curr->use_min = curr->use_max = 1;
	curr->min = 200;
	curr->max = 2000;
	curr->ch = -1;
}  // add_rr_axis()


void
main_window::add_syst_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	curr->event_class_idx = curr_clh_idx;
	strcpy(curr->prop_name, "rr-systolic");
	curr->event_property = prop;
	strcpy(curr->unit, "mmHg");
	curr->use_min = curr->use_max = 1;
	curr->min = 0;
	curr->max = 200;

	// bloodpressure values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh[curr_clh_idx], RA_CH_TYPE_RR, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_syst_axis()


void
main_window::add_dias_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	curr->event_class_idx = curr_clh_idx;
	strcpy(curr->prop_name, "rr-diastolic");
	curr->event_property = prop;
	strcpy(curr->unit, "mmHg");
	curr->use_min = curr->use_max = 1;
	curr->min = 0;
	curr->max = 200;

	// bloodpressure values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh[curr_clh_idx], RA_CH_TYPE_RR, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_dias_axis()


void
main_window::add_resp_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	curr->event_class_idx = curr_clh_idx;
	strcpy(curr->prop_name, "resp-chest-mean-rri");
	curr->event_property = prop;
	strcpy(curr->unit, "%");
	curr->use_min = curr->use_max = 1;
	curr->min = -1;
	curr->max = 1;

	// respiration values are channel dependent
	long *ch, num_ch;
	if ((get_channel(opt->clh[curr_clh_idx], RA_CH_TYPE_RESP, &ch, &num_ch) == 0) && (num_ch > 0))
		curr->ch = ch[0];
	else
		curr->ch = -1;
	if (ch)
		delete[] ch;
}  // add_resp_axis()


void
main_window::add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, const char *name,
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
main_window::get_channel(class_handle clh, long ch_type, long **ch, long *num_ch)
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


void
main_window::slot_exit()
{
	close_all_measurements();
	close();
} // slot_exit()


void
main_window::slot_auto_pos(bool toggle)
{
	do_auto_pos = toggle;
	auto_pos_act->setChecked(do_auto_pos);
} // slot_auto_pos()


void
main_window::slot_layout(int idx)
{
	if (idx == current_layout)
		return;
	
	current_layout = idx;
	resizeEvent(NULL);
} // slot_layout()


void
main_window::slot_recent_files(int idx)
{
	open_signal_file(recent_used_files[idx].c_str());
} // slot_recent_files()
