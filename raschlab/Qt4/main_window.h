#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QActionGroup>
#include <ra.h>
#include <ra_plot_structs.h>
#include <pl_general.h>		/* needed for ra_alloc_mem/ra_free_mem when init plots; find more
				   correct way to do it without the need to include this header file
				   (maybe by moving mem functions to ra.h) */
#include <vector>

using namespace std;


#include "settings.h"
#include "signal_ra_qt.h"

#define RASCHLAB_VERSION   "0.3.0"

struct single_view_layout
{
	int view_id;

	double vert_pos;
	double horz_pos;

	double height;
	double width;

	// TODO: add support for plot-views showing different data
	// (e.g. one showing tachograms, another one showing other stuff)
};  /* struct single_view_layout */


struct view_layout
{
	QString name;
	QString desc;

	vector<struct single_view_layout *> views;
}; /* struct view_layout */


class QAction;
class QMenu;
class QWorkspace;
class QSignalMapper;

class main_window : public QMainWindow
{
	Q_OBJECT
		
 public:
	main_window(ra_handle h);

	void open_signal_file(QString file);
	
 protected:
//	void close_event(QCloseEvent *event);
	
 private slots:
	void open();
	void save();
	void about();
	void update_menus();
	void update_window_menu();
	void closeEvent(QCloseEvent * event);
	void close_active_measurement();
	void close_all_measurements();
	void slot_exit();
	void slot_recent_files(int idx);

	// general menu entries
	void slot_plugins();
	void slot_eval();
	void slot_save_eval_ascii(bool toggle);

	// process menu (will be generated dynamically, later)
	void slot_detect();
	void slot_detect_ctg();
	void slot_rr_morphology();  // for debug of rr_morphology plugin
	void slot_classify_ecg();
	void slot_calc();

	// layout menu entries
	void slot_layout(int idx);
	void slot_auto_pos(bool toogle);

	virtual void resizeEvent(QResizeEvent *ev);

 private:
	void init_view_layouts();
	void create_actions();
	void create_general_actions();
	void create_detect_actions();
	void create_menus();
	void create_view_menu();
	void update_detect_menu(signal_ra_qt *sig);
	void create_toolbars();
	void create_statusbar();
	void set_recent_used_files();

	QString select_signal();
	int create_view(signal_ra_qt *sig);
	QWidget *show_qrs_templates(signal_ra_qt *sig, sum_handle sh);
	QWidget *show_hrt_view(signal_ra_qt *sig);
	QWidget *show_uc_templates(signal_ra_qt *sig, sum_handle sh);

	signal_ra_qt *active_signal();
	void process_signal(const char *plugin_name, bool save_eval = false);

	bool fill_plot_infos(signal_ra_qt *sig, struct ra_plot_options *opt);
	struct ra_plot_axis *add_axis(struct ra_plot_options *opt);
	void add_pos_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop);
	void add_rri_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop);
	void add_syst_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop);
	void add_dias_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop);
	void add_resp_axis(struct ra_plot_options *opt, int curr_clh_idx, prop_handle prop);
	void add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, const char *name,
		      long r, long g, long b);
	int get_channel(class_handle clh, long ch_type, long **ch, long *num_ch);

	QWorkspace *workspace;
	QSignalMapper *window_mapper;
	QSignalMapper *layout_mapper;
	QSignalMapper *recent_files_mapper;

	QMenu *file_menu;
	QMenu *recent_files_menu;
	QMenu *general_menu;
	QMenu *detect_menu;
	QMenu *view_menu;
	QMenu *window_menu;
	QMenu *help_menu;

	QToolBar *file_toolbar;
	QToolBar *edit_toolbar;

	QAction *open_act;
	QAction *save_act;
	QAction *exit_act;
	QAction *close_act;
	QAction *close_all_act;
	QAction *tile_act;
	QAction *cascade_act;
	QAction *arrange_act;
	QAction *next_act;
	QAction *previous_act;
	QAction *separator_act;
	QAction *about_act;
	QAction *about_qt_act;

	// general menu
	QAction *general_plugin_act;
	QAction *general_eval_act;
	QAction *save_eval_ascii_act;

	// detect menu
	QAction *do_detect;
	QAction *do_detect_ctg;
	QAction *do_rr_morph;
	QAction *do_ecg_class;
	QAction *do_calc;

	// view menu
	QAction *auto_pos_act;
	QActionGroup *layout_act_group;

	ra_handle ra;
	raschlab_settings settings;
	char last_path[MAX_PATH_RA];

	// List of all open ra-signals. The list will be checked for modified
	// or created evaluations when app will be closed.
	QList<signal_ra_qt *> signal_list;

	vector<struct view_layout *> view_layouts;
	int current_layout;
	bool do_auto_pos;

	vector<string> recent_used_files;
}; /* class main_window */

#endif  // MAIN_WINDOW_H
