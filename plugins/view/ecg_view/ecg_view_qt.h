/*----------------------------------------------------------------------------
 * ecg_view_qt.h
 *
 *
 *
 * Author(s): Raphael Schneider (librasch@gmail.com)
 *
 * Copyright (C) 2002-2010, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#ifndef _ECG_VIEW_QT_H
#define _ECG_VIEW_QT_H

#include "ecg_view_general.h"
#include "../ts_view/ts_view_qt.h"

#include <QtGui/QComboBox>
#include <QtGui/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QPaintEvent>
#include <QtCore/QSignalMapper>

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4250)
#endif

class ecg_view_qt:public ts_view_qt, public ecg_view_general
{
	Q_OBJECT
public:
	ecg_view_qt(meas_handle h, int num_ch, int *ch, QWidget * p = 0, char *name = 0, Qt::WFlags f = 0);
	~ecg_view_qt();

	virtual void do_update();

public slots:
	void slot_set_area_to_artifacts();
	void slot_set_area_to_normals();
	void slot_set_area_to_pacs();
	void slot_set_area_to_pvcs();
	void slot_set_area_to_paced();
	void slot_remove_beats_in_area();

	void sort_general(prop_handle ph, double value);
	void sort_template(int templ_num);
	void sort_ch_found(int i);
	void sort_rri(int short_first);
	void sort_class(int classification);

	void slot_process() { post_process(); }
	void slot_toggle_edit_mode();
	void slot_toggle_show_morph_pos();
	void slot_toggle_show_p_wave_pos();
	void slot_toggle_show_qrs_pos();
	void slot_toggle_show_t_wave_pos();

	void slot_morphology_nok();
	void slot_p_wave_nok();
	void slot_qrs_nok();
	void slot_t_wave_nok();

	void slot_ev_morphology_nok();
	void slot_ev_p_wave_nok();
	void slot_ev_qrs_nok();
	void slot_ev_t_wave_nok();

	void slot_ch_morphology_nok();
	void slot_ch_p_wave_nok();
	void slot_ch_qrs_nok();
	void slot_ch_t_wave_nok();

	void slot_p_type_changed(int);
	void slot_qrs_type_changed(int);
	void slot_t_type_changed(int);

	void slot_edit_apply();
	void slot_edit_apply_all();
	void slot_edit_close();

	void slot_show_low_pass_signal();
	void slot_show_high_pass_signal();

 protected:
	virtual void paintEvent(QPaintEvent *ev);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void resizeEvent(QResizeEvent *ev);
	virtual void mouseDoubleClickEvent(QMouseEvent * ev);
	virtual void keyPressEvent(QKeyEvent * ev);

	void plot_beats(QPainter *p);
	void show_beat_class(QPainter *p, long ev_id, long x);
	void show_beat_template(QPainter *p, long ev_id, long x);

	void show_beat_position(QPainter *p, long ev_id);
	void show_p_wave_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area);
	void show_qrs_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area);
	void show_t_wave_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area);

	void show_beat_rri(QPainter *p, long ev_id, long x_1, long x);
	void show_beat_info(QPainter *p, long ev_id, long x);
	void show_beat_index(QPainter *p, long ev_id, long x);

	void plot_cursor(void *p);

	void show_pos(QPainter *p, int ch_idx, int x, long pos, QColor col = QColor(0, 0, 0));
	void show_peak(QPainter *p, int ch_idx, int x, long pos, bool below, QColor col = QColor(0, 0, 0));
	void show_events(QPainter *p);

	void plot_edit_area(QPainter *p);
	void print_edit_beat_info(QPainter *p);
	void show_pos_edit(QPainter *p, long pos, QColor col = QColor(0, 0, 0), const char *type=NULL, bool type_above=false);
	void show_peak_edit(QPainter *p, long pos, bool below, QColor col = QColor(0, 0, 0));

	/* additional entries in marked area menu */
	QAction *set_artifacts;
	QAction *set_normal;
	QAction *set_pacs;
	QAction *set_pvcs;
	QAction *set_paced;
	QAction *remove_beats;

	/* top ECG menu */
	QMenu *_popup_menu_ecg;
	QAction *edit_mode_act;
	QAction *show_morph_act;
	QAction *show_p_wave_act;
	QAction *show_qrs_act;
	QAction *show_t_wave_act;

	/* sort menu */
	QMenu *_popup_menu_sort;
	QSignalMapper *sort_ch_mapper;
	QSignalMapper *sort_rri_mapper;
	QSignalMapper *sort_template_mapper;
	QSignalMapper *sort_annot_mapper;

	/* menu shown when in edit mode */
	QMenu *_popup_menu_edit;
	QAction *show_lowpass_act;
	QAction *show_highpass_act;


	QMenu *_popmenu_edit_channel;
	int _ed_ch_morph_nok;
	int _ed_ch_p_nok;
	int _ed_ch_qrs_nok;
	int _ed_ch_t_nok;
	QMenu *_popmenu_edit_event;
	int _ed_ev_morph_nok;
	int _ed_ev_p_nok;
	int _ed_ev_qrs_nok;
	int _ed_ev_t_nok;
	QMenu *_popmenu_edit_single;
	int _ed_morph_nok;
	int _ed_p_nok;
	int _ed_qrs_nok;
	int _ed_t_nok;

	QComboBox *_p_type_select;
	QComboBox *_qrs_type_select;
	QComboBox *_t_type_select;
	QPushButton *_edit_apply;
	QPushButton *_edit_apply_all;
	QPushButton *_edit_close;
};  // class ecg_view_qt

#ifdef WIN32
#pragma warning(pop)
#endif

#endif // _ECG_VIEW_QT_H
