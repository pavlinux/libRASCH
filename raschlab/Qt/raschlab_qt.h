/***************************************************************************
                          raschlab_qt.h  -  description
                             -------------------
    begin                : Sat Nov 3 2001
    copyright            : (C) 2001-2004 by Raphael Schneider
    email                : rasch@med1.med.tum.de

    $Header: /home/cvs_repos.bak/librasch/raschlab/Qt/raschlab_qt.h,v 1.7 2004/07/01 13:17:55 rasch Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _RASCHLAB_QT_H
#define _RASCHLAB_QT_H


// include files for QT
#include <QtGui/qmainwindow.h>

#include <Qt/qsignalmapper.h>
#include <QtGui/qaction.h>
#include <QtGui/qworkspace.h>
#include <QtGui/qmenubar.h>
#include <QtGui/qtoolbar.h>
#include <QtGui/qstatusbar.h>
#include <QtGui/qwhatsthis.h>
#include <QtGui/qmenu.h>
#include <QtGui/qtoolbutton.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qprinter.h>
#include <Qt/qstring.h>
#include <QtGui/qpixmap.h>
#include <QtGui/QResizeEvent>
#include <QtGui/QActionGroup>

extern "C" {
#include <ra.h>
}

#include "signal_ra_qt.h"
#include "../general/settings.h"


// forward declaration
class signals_qt;

class raschlab_qt : public QMainWindow
{
	Q_OBJECT

public:
	raschlab_qt(ra_handle ra);
	~raschlab_qt();

	// opens a file specified by commandline option
	void open_signal_file(const char *file = 0);
  	
protected:
	QString select_signal();
	void create_view(signal_ra_qt *sig);
	QWidget *show_qrs_templates(signal_ra_qt *sig, sum_handle sh);
	QWidget *show_uc_templates(signal_ra_qt *sig, sum_handle sh);

	signal_ra_qt *active_signal();

	void process_signal(char *plugin_name);
		
	virtual void resizeEvent(QResizeEvent *e);

	bool fill_plot_infos(signal_ra_qt *sig, struct ra_plot_options *opt);
	struct ra_plot_axis *add_axis(struct ra_plot_options *opt);
	void add_pos_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_rri_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_syst_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_dias_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_resp_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, char *name,
		long r, long g, long b);
	int get_channel(class_handle clh, long ch_type, long **ch, long *num_ch);

	// user specific settings
	settings _settings;

  	ra_handle _ra;
	char _last_path[MAX_PATH_RA];
private slots:
	// file menu
	void slot_open();
	void slot_save();
	void slot_close();
	void slot_quit();

	// general menu
	void slot_plugins();
	void slot_eval();
	void slot_save_eval_ascii(bool toggle);
	
	// process menu (will be generated dynamically, later)
	void slot_detect();
	void slot_detect_ctg();
	void slot_rr_morphology();  // for debug of rr_morphology plugin
	void slot_classify_ecg();
	void slot_calc();

	// view menu
	void slot_show_toolbar(bool toggle);
	void slot_show_statusbar(bool toggle);
	void slot_show_plot();

	// window menu
	void slot_new_window();  // FIXME: here or in general-menu
	// will be called when window-menu is about to show,
	// window list is created here
	void window_menu_about_to_show();
	// is called when window in window-menu is selected
	void window_menu_activated(int id);
	
	// help menu
	void slot_about();

	// non-menu related slots
		
private:
	void create_actions();
	void create_menus();
	void create_toolbars();
	void create_statusbar();
	void make_detect_menu(signal_ra_qt *sig);

	// menus
	QMenu *file_menu;
	QMenu *general_menu;
	QMenu *detect_menu;  // FIXME: will be created dynamically
	QMenu *view_menu;
	QMenu *help_menu;
	QMenu *window_menu;

	// toolbars
	QToolBar *file_toolbar;

	// MDI frame widget (init in init_view())
	QWorkspace *workspace;
	QSignalMapper *window_mapper;

	QWidget *view;
	QString path;

     	// List of all open ra-signals. The list will be checked for modified
     	// or created evaluations when app will be closed.
	QList<signal_ra_qt *> signal_list;

	// actions
	// file menu
	QAction *file_open;
	QAction *file_save;
	QAction *file_close;
	QAction *file_quit;

	// general menu
	QAction *general_plugin;
	QAction *general_eval;
	QAction *save_eval_ascii;

	// detect menu
	QAction *do_detect;
	QAction *do_detect_ctg;
	QAction *do_rr_morph;
	QAction *do_ecg_class;
	QAction *do_calc;

	// view menu
	QAction *view_toolbar;
	QAction *view_statusbar;
	QAction *view_plot;

	// window menu
	QActionGroup *window_action;
	QAction *window_new;
	QAction *window_tile;
	QAction *window_cascade;

	// help menu
	QAction *help_about;
};  // class raschlab_qt

#endif  // _RASCHLAB_QT_H


