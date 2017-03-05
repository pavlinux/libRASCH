#ifndef PAIR_OPT_DLG_QT_h
#define PAIR_OPT_DLG_QT_h

#include "pair_opt_dlg_ui.h"

#include <ra_plot_structs.h>
#include <stdlib.h>
#include <pl_general.h>


class pair_opt_dlg_qt : public QDialog, public Ui::pair_opt_dlg
{
	Q_OBJECT
public:
	pair_opt_dlg_qt(meas_handle meas, struct ra_plot_options *opt, int curr_pair = 0);
	~pair_opt_dlg_qt();


protected:
	void _copy_opt_struct(struct ra_plot_options *dest, struct ra_plot_options *src);
	void _copy_pair_struct(struct ra_plot_pair **dest, struct ra_plot_pair *src, int n);
	void _copy_axis_struct(struct ra_plot_axis **dest, struct ra_plot_axis *src, int n);
	void _free_opt_struct(struct ra_plot_options *opt);

	meas_handle _meas;
	struct ra_plot_pair *_curr_pair;
	struct ra_plot_axis *_curr_axis;
	struct ra_plot_options *_opt;
	struct ra_plot_options *_opt_orig;

	int _plot_num_symbol;
	int _plot_num_line;
	int _plot_num_time_format;
	int _plot_num_log_type;

	int _num_ch;
	long *_ch;

	QPalette symbol_color_palette;
	QPalette line_color_palette;

protected slots:
	void _init();
	void _do_connect();

	void _save_pair_data();
	void _save_axis_data();

	void _pairs_select(int index);

	void _plot_symbol_click();
	void _symbol_select(int index);
	void _sym_color_click();

	void _plot_line_click();
	void _line_select(int index);
	void _line_color_click();

//    void _add_analyses_click();

	void _axis_init();

	void _axis_select(int index);
	void _channel_select(int index);

	void _use_min_click();
	void _use_max_click();

	void _is_time_click();
	void _time_select(int index);
	void _plot_log_click();
	void _log_select(int index);

	void _cancel_click();
	void _ok_click();
}; // class pair_opt_dlg_qt


#endif // PAIR_OPT_DLG_QT_h
