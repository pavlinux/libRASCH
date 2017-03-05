/*----------------------------------------------------------------------------
 * ecg_view_qt.cpp
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

#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QPaintEvent>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include <ra_ecg.h>
#include <ra_detect.h>

#include "ecg_view_qt.h"

#define SHOW_BEAT_DEBUG_INFO  0


ecg_view_qt::ecg_view_qt(meas_handle h, int num_ch, int *ch, QWidget *p/*=0*/, char *name/*=0*/, Qt::WFlags f/*=0*/)
	: ts_view_general(h, num_ch, ch),
	  ts_view_qt(h, num_ch, ch, p, name, f),
	  ecg_view_general(h)
{
	setFocusPolicy(Qt::StrongFocus);

	QAction *act;

	// -------------------- add marked-area entries --------------------
	set_artifacts = _popup_menu_marked_area->addAction(QString(gettext("mark beats as artifacts")),
							   this, SLOT(slot_set_area_to_artifacts()));
	set_normal = _popup_menu_marked_area->addAction(QString(gettext("mark beats as normal beats")),
							   this, SLOT(slot_set_area_to_normal()));
	set_pacs = _popup_menu_marked_area->addAction(QString(gettext("mark beats as premature normal beats")),
							   this, SLOT(slot_set_area_to_pacs()));
	set_pvcs = _popup_menu_marked_area->addAction(QString(gettext("mark beats as ventricular beats")),
							   this, SLOT(slot_set_area_to_pvcs()));
	set_paced = _popup_menu_marked_area->addAction(QString(gettext("mark beats as paced beats")),
							   this, SLOT(slot_set_area_to_paced()));
	remove_beats = _popup_menu_marked_area->addAction(QString(gettext("remove beats")),
							   this, SLOT(slot_remove_beats_in_area()));

	// -------------------- ecg specific sub-menue  --------------------
	_popup_menu_ecg = _popup_menu->addMenu(QString(gettext("ecg")));

	// -------------------- add show entries --------------------
	edit_mode_act = _popup_menu_ecg->addAction(QString(gettext("Edit Mode")), this, SLOT(slot_toggle_edit_mode()));
	edit_mode_act->setCheckable(true);
	edit_mode_act->setEnabled(false);

	act = _popup_menu_ecg->addAction(QString(gettext("update ecg values")), this, SLOT(slot_process()));
	show_morph_act = _popup_menu_ecg->addAction(QString(gettext("show positions")),
						    this, SLOT(slot_toggle_show_morph_pos()));
	show_morph_act->setCheckable(true);
	show_morph_act->setChecked(_show_morph_pos);

	show_p_wave_act = _popup_menu_ecg->addAction(QString(gettext("show p-wave positions")),
						     this, SLOT(slot_toggle_show_p_wave_pos()));
	show_p_wave_act->setCheckable(true);
	show_p_wave_act->setChecked(_show_p_wave_pos);

	show_qrs_act = _popup_menu_ecg->addAction(QString(gettext("show qrs positions")),
						     this, SLOT(slot_toggle_show_qrs_pos()));
	show_qrs_act->setCheckable(true);
	show_qrs_act->setChecked(_show_qrs_pos);

	show_t_wave_act = _popup_menu_ecg->addAction(QString(gettext("show t-wave positions")),
						     this, SLOT(slot_toggle_show_t_wave_pos()));
	show_t_wave_act->setCheckable(true);
	show_t_wave_act->setChecked(_show_t_wave_pos);

	// -------------------- add sort entries --------------------
	_popup_menu_sort = _popup_menu_ecg->addMenu(QString(gettext("sort")));
	sort_ch_mapper = new QSignalMapper(this);
	for (int i = 0; i < _num_ch; i++)
	{
		char t[100];
		sprintf(t, gettext("beats found in %d channel%s"), (i+1), ((i+1) > 1 ? "s" : ""));
		act = _popup_menu_sort->addAction(t);
		connect(act, SIGNAL(triggered()), sort_ch_mapper, SLOT(map()));
		sort_ch_mapper->setMapping(act, i);
	}
	connect(sort_ch_mapper, SIGNAL(mapped(int)), this, SLOT(sort_ch_found(int)));

	_popup_menu_sort->addSeparator();

	sort_rri_mapper = new QSignalMapper(this);
	act = _popup_menu_sort->addAction(QString(gettext("shortest rr-interval first")));
	connect(act, SIGNAL(triggered()), sort_rri_mapper, SLOT(map()));
	sort_rri_mapper->setMapping(act, 1);
	act = _popup_menu_sort->addAction(QString(gettext("longets rr-interval first")));
	connect(act, SIGNAL(triggered()), sort_rri_mapper, SLOT(map()));
	sort_rri_mapper->setMapping(act, 0);
	connect(sort_rri_mapper, SIGNAL(mapped(int)), this, SLOT(sort_rri(int)));

	_popup_menu_sort->addSeparator();

	sort_annot_mapper = new QSignalMapper(this);
	act = _popup_menu_sort->addAction(QString(gettext("normal_beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, ECG_CLASS_SINUS);
	act = _popup_menu_sort->addAction(QString(gettext("PVCs")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, ECG_CLASS_VENT);
	act = _popup_menu_sort->addAction(QString(gettext("paced beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, ECG_CLASS_PACED);
	act = _popup_menu_sort->addAction(QString(gettext("atrial paced beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, (ECG_CLASS_PACED | (ECG_CLASS_PACED_ATRIUM << 16)));
	act = _popup_menu_sort->addAction(QString(gettext("ventricular paced beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, (ECG_CLASS_PACED | (ECG_CLASS_PACED_VENTRICLE << 16)));
	act = _popup_menu_sort->addAction(QString(gettext("atrial & vent. paced beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, (ECG_CLASS_PACED
										| (ECG_CLASS_PACED_VENTRICLE << 16)
										| (ECG_CLASS_PACED_ATRIUM << 16)));
	act = _popup_menu_sort->addAction(QString(gettext("unknown beats")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, ECG_CLASS_UNKNOWN);
	act = _popup_menu_sort->addAction(QString(gettext("Artifacts")));
	connect(act, SIGNAL(triggered()), sort_annot_mapper, SLOT(map()));
	sort_annot_mapper->setMapping(act, ECG_CLASS_ARTIFACT);
	connect(sort_annot_mapper, SIGNAL(mapped(int)), this, SLOT(sort_class(int)));

	_popup_menu_sort->addSeparator();

	if (_num_templates > 0)
	{
		sort_template_mapper = new QSignalMapper(this);
		for (int i = 0; i < _num_templates; i++)
		{
			char t[100];
			sprintf(t, gettext("beats in template %d"), i);
			act = _popup_menu_sort->addAction(t);
			connect(act, SIGNAL(triggered()), sort_template_mapper, SLOT(map()));
			sort_ch_mapper->setMapping(act, i);
		}
		connect(sort_template_mapper, SIGNAL(mapped(int)), this, SLOT(sort_template(int)));
	}
	
	_popup_menu_edit = new QMenu();
	show_lowpass_act = _popup_menu_edit->addAction(QString(gettext("Show low-pass filtered signal")),
						     this, SLOT(slot_show_low_pass_signal()));
	show_lowpass_act->setCheckable(true);
	show_highpass_act = _popup_menu_edit->addAction(QString(gettext("Show high-pass filtered signal")),
						      this, SLOT(slot_show_high_pass_signal()));
	show_highpass_act->setCheckable(true);


/*

	_popup_menu_ecg = new Q3PopupMenu();
	_popup_menu_ecg->setCheckable(true);

	_edit_mode_id = _popup_menu_ecg->insertItem(QString(gettext("Edit Mode")),
						 this, SLOT(slot_toggle_edit_mode()));
	// no event selected at the beginning -> disable menu entry
	_popup_menu_ecg->setItemEnabled(_edit_mode_id, false);

	_update_id = _popup_menu_ecg->insertItem(QString(gettext("update ecg values")),
						 this, SLOT(slot_process()));
	_show_morph_menu_id = _popup_menu_ecg->insertItem(QString(gettext("show positions")),
						     this, SLOT(slot_toggle_show_morph_pos()));
	_popup_menu_ecg->setItemChecked(_show_morph_menu_id, _show_morph_pos);

	_show_p_wave_menu_id = _popup_menu_ecg->insertItem(QString(gettext("show p-wave positions")),
						     this, SLOT(slot_toggle_show_p_wave_pos()));
	_popup_menu_ecg->setItemChecked(_show_p_wave_menu_id, _show_p_wave_pos);

	_show_qrs_menu_id = _popup_menu_ecg->insertItem(QString(gettext("show qrs positions")),
						     this, SLOT(slot_toggle_show_qrs_pos()));
	_popup_menu_ecg->setItemChecked(_show_qrs_menu_id, _show_qrs_pos);

	_show_t_wave_menu_id = _popup_menu_ecg->insertItem(QString(gettext("show t-wave positions")),
						     this, SLOT(slot_toggle_show_t_wave_pos()));
	_popup_menu_ecg->setItemChecked(_show_t_wave_menu_id, _show_t_wave_pos);

	_popmenu_edit_channel = new Q3PopupMenu();
	_ed_ch_morph_nok = _popmenu_edit_channel->insertItem(
		QString(gettext("Remove morphology data for the complete channel")),
		this, SLOT(slot_ch_morphology_nok()));
	_ed_ch_p_nok = _popmenu_edit_channel->insertItem(
		QString(gettext("Remove P-wave data for the complete channel")),
		this, SLOT(slot_ch_p_wave_nok()));
	_ed_ch_qrs_nok = _popmenu_edit_channel->insertItem(
		QString(gettext("Remove QRS complex data for the complete channel")),
		this, SLOT(slot_ch_qrs_nok()));
	_ed_ch_t_nok = _popmenu_edit_channel->insertItem(
		QString(gettext("Remove T-wave data for the complete channel")),
		this, SLOT(slot_ch_t_wave_nok()));

	_popmenu_edit_event = new Q3PopupMenu();
	_ed_ev_morph_nok = _popmenu_edit_event->insertItem(
		QString(gettext("Remove morphology data for the current event in all channels")),
		this, SLOT(slot_ev_morphology_nok()));
	_ed_ev_p_nok = _popmenu_edit_event->insertItem(
		QString(gettext("Remove P-wave data for the current event in all channels")),
		this, SLOT(slot_ev_p_wave_nok()));
	_ed_ev_qrs_nok = _popmenu_edit_event->insertItem(
		QString(gettext("Remove QRS complex data for the current event in all channels")),
		this, SLOT(slot_ev_qrs_nok()));
	_ed_ev_t_nok = _popmenu_edit_event->insertItem(
		QString(gettext("Remove T-wave data for the current event in all channels")),
		this, SLOT(slot_ev_t_wave_nok()));

	_popmenu_edit_single = new Q3PopupMenu();
	_ed_morph_nok = _popmenu_edit_single->insertItem(
		QString(gettext("Remove morphology data for the current event")),
		this, SLOT(slot_morphology_nok()));
	_ed_p_nok = _popmenu_edit_single->insertItem(
		QString(gettext("Remove P-wave data for the current event")),
		this, SLOT(slot_p_wave_nok()));
	_ed_qrs_nok = _popmenu_edit_single->insertItem(
		QString(gettext("Remove QRS complex data for the current event")),
		this, SLOT(slot_qrs_nok()));
	_ed_t_nok = _popmenu_edit_single->insertItem(
		QString(gettext("Remove T-wave data for the current event")),
		this, SLOT(slot_t_wave_nok()));

	_popup_menu_ecg->insertSeparator();

	_popup_menu_ecg->insertItem(QString(gettext("sort")), _popup_menu_sort);
	if (_num_templates > 0)
		_popup_menu_ecg->insertItem(QString(gettext("templates")), _popup_menu_template);

	_popup_menu->insertItem(QString(gettext("ecg")), _popup_menu_ecg);
*/
	/* ------------------------------ init edit-beat area widgets ------------------------------ */
	_p_type_select = new QComboBox(this);
	_p_type_select->setFocusPolicy(Qt::NoFocus);
	_p_type_select->addItem("---");
	// !!!!! keep order of combox entries !!!!!
	// (TODO: assign values to entries to be independent of order)
	_p_type_select->addItem(QString(gettext("negative")));
	_p_type_select->addItem(QString(gettext("positive")));
	_p_type_select->addItem(QString(gettext("bi-phasic (neg/pos)")));
	_p_type_select->addItem(QString(gettext("bi-phasic (pos/neg)")));
	_p_type_select->addItem(QString(gettext("unknown")));
	_p_type_select->hide();
	_p_type_select->setMinimumWidth(200);
	connect(_p_type_select, SIGNAL(activated(int)), this, SLOT(slot_p_type_changed(int)));

	_qrs_type_select = new QComboBox(this);
	_qrs_type_select->setFocusPolicy(Qt::NoFocus);
	_qrs_type_select->addItem("---");
	_qrs_type_select->addItem(QString(gettext("S")));
	_qrs_type_select->addItem(QString(gettext("R")));
	_qrs_type_select->addItem(QString(gettext("QR")));
	_qrs_type_select->addItem(QString(gettext("RS")));
	_qrs_type_select->addItem(QString(gettext("QRS")));
	_qrs_type_select->addItem(QString(gettext("RSR'")));
	_qrs_type_select->addItem(QString(gettext("unknown")));
	_qrs_type_select->hide();
	_qrs_type_select->setMinimumWidth(200);
	connect(_qrs_type_select, SIGNAL(activated(int)), this, SLOT(slot_qrs_type_changed(int)));

	_t_type_select = new QComboBox(this);
	_t_type_select->setFocusPolicy(Qt::NoFocus);
	_t_type_select->addItem("---");
	_t_type_select->addItem(QString(gettext("negative")));
	_t_type_select->addItem(QString(gettext("positive")));
	_t_type_select->addItem(QString(gettext("bi-phasic (neg/pos)")));
	_t_type_select->addItem(QString(gettext("bi-phasic (pos/neg)")));
	_t_type_select->addItem(QString(gettext("unknown")));
	_t_type_select->hide();
	_t_type_select->setMinimumWidth(200);
	connect(_t_type_select, SIGNAL(activated(int)), this, SLOT(slot_t_type_changed(int)));

	_edit_apply = new QPushButton(QString(gettext("Apply")), this);
	_edit_apply->setFocusPolicy(Qt::NoFocus);
	_edit_apply->hide();
	connect(_edit_apply, SIGNAL(pressed()), this, SLOT(slot_edit_apply()));

	_edit_apply_all = new QPushButton(QString(gettext("Apply All")), this);
	_edit_apply_all->setFocusPolicy(Qt::NoFocus);
	_edit_apply_all->hide();
	connect(_edit_apply_all, SIGNAL(pressed()), this, SLOT(slot_edit_apply_all()));

	_edit_close = new QPushButton(QString(gettext("Close")), this);
	_edit_close->setFocusPolicy(Qt::NoFocus);
	_edit_close->hide();
	connect(_edit_close, SIGNAL(pressed()), this, SLOT(slot_edit_close()));
} // constructor


ecg_view_qt::~ecg_view_qt()
{
	delete _popup_menu_sort;
	delete _popup_menu_ecg;
} // destructor


void
ecg_view_qt::do_update()
{
	get_eventtypes();
} // do_update()


void
ecg_view_qt::paintEvent(QPaintEvent * ev)
{
	if (!_in_edit_mode)
	{
		_p_type_select->hide();
		_qrs_type_select->hide();
		_t_type_select->hide();

		_edit_apply->hide();
		_edit_apply_all->hide();
		_edit_close->hide();
	}

	ts_view_qt::paintEvent(ev);

	QPainter p(this);

	plot_beats(&p);

	show_events(&p);

	// status bar
	if (_ts_opt.show_status && _num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		p.drawText(_left_space, this->height() - 19,
			   this->width() - _left_space - _right_space - 4, 15, Qt::AlignRight, t);
	}

	if (_cursor_pos >= 0)
		plot_cursor((void *)&p);

	if (_in_edit_mode)
		plot_edit_area(&p);
	else
	{
		_edit_apply->setEnabled(false);
		_edit_apply_all->setEnabled(false);
	}
} // paintEvent()


void
ecg_view_qt::plot_beats(QPainter *p)
{
	if (_num_events == 0)
		return;

	int *x = NULL;
	int num = 0;
	long *events = NULL;
	get_beat_pos(&x, &events, &num);

	int w, h;
	get_text_extent(p, "xX", &w, &h);
	// get mean difference between 20 beats (better behaviour when SVPCs occure)
	int n = num >= 20 ? 20 : num;
	int d = 0;
	int i;  // because of VC++
	for (i = 1; i < n; i++)
		d += (x[i] - x[i-1]);
	if (n > 1)
		d /= (n-1);
	else
		d = 100;
	// check if there is even no place for the annotation
	if (d < w)
	{
		if (x)
			delete[]x;
		if (events)
			delete[]events;
		return;
	}

	for (int i = 0; i < num; i++)
	{
		show_beat_class(p, events[i], x[i]);

		// if fast speed -> show only annotation
 		if (d < (3 * w))
 			continue;

		show_beat_template(p, events[i], x[i]);
		show_beat_position(p, events[i]);
		
		if (i > 0)
			show_beat_rri(p, events[i], x[i - 1], x[i]);
			
#if SHOW_BEAT_DEBUG_INFO
		show_beat_info(p, events[i], x[i]);
		show_beat_index(p, events[i], x[i]+20);
#endif // SHOW_BEAT_DEBUG_INFO
	}

	if (x)
		delete[]x;
	if (events)
		delete[]events;
} // plot_beats()


void
ecg_view_qt::show_beat_class(QPainter *p, long ev_id, long x)
{
	if (!_prop_class)
		return;

	// show beat classification
	ra_prop_get_value(_prop_class, ev_id, -1, _vh);
	int cl = (int)ra_value_get_long(_vh);
	const char *c = raecg_get_class_string(cl);
	p->drawText(x - 10, 2, 20, 15, Qt::AlignHCenter, c);
} // show_beat_class()


int
ecg_view_qt_cmp_long(const void *p1, const void *p2)
{
	return ((*(long *)p1) - (*(long *)p2));
} /* ecg_view_qt_cmp_long() */


void
ecg_view_qt::show_beat_template(QPainter *p, long ev_id, long x)
{
	if (_num_templates <= 0)
		return;

	long templ_num = -1;
	for (long l = 0; l < _num_templates; l++)
	{
		if (bsearch(&ev_id, _templates[l].event_ids, _templates[l].num_events,
			    sizeof(long), ecg_view_qt_cmp_long) != NULL)
		{
			templ_num = _templates[l].part_id;
			break;
		}
	}

	QString s;
	s = s.setNum(templ_num);
	p->drawText(x - 30, 17, 60, 30, Qt::AlignHCenter, s);
} // show_beat_template()


void
ecg_view_qt::show_beat_position(QPainter *p, long ev_id)
{
	if (!_prop_pos || !_show_morph_pos)
		return;

	int cl = -1;
	if (_prop_class)
	{
		ra_prop_get_value(_prop_class, ev_id, -1, _vh);
		cl = (int)ra_value_get_long(_vh);
	}

	// show positions
	for (int j = 0; j < _num_ch; j++)
	{
		long ch_num = _ch[j].ch_number;

		if (!_ch[j].show_it || (_ch[j].type != RA_CH_TYPE_ECG))
			continue;

		if (_prop_ch)
		{
			long curr = 1L << ch_num;
			ra_prop_get_value(_prop_ch, ev_id, -1, _vh);
			long ch_found = ra_value_get_long(_vh);
			if (!(ch_found & curr))
				continue;
		}

		// offsets are relative to the start of the event
		long start, end;
		ra_class_get_event_pos(_clh, ev_id, &start, &end);
		int qrs_pos = start;

		if (!_prop_morph_flags)
			continue;
		if (ra_prop_get_value(_prop_morph_flags, ev_id, ch_num, _vh) != 0)
			continue;
		long morph_flags = ra_value_get_long(_vh);

		if (!IS_ARTIFACT(cl) && !IS_UNKNOWN(cl))
		{
			if ((cl != -1) && IS_SINUS(cl))
				show_p_wave_info(p, ev_id, j, qrs_pos, morph_flags, false);
			show_qrs_info(p, ev_id, j, qrs_pos, morph_flags, false);
			show_t_wave_info(p, ev_id, j, qrs_pos, morph_flags, false);
		}
	}
} // show_beat_position()


void
ecg_view_qt::show_p_wave_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area)
{
	if (!_prop_p_start || !_prop_p_end || !_show_p_wave_pos)
		return;

	if ((morph_flags & ECG_P_WAVE_OK) == 0)
		return;

	if (ra_prop_get_value(_prop_p_start, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.p_start;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(p, pos, QColor(255, 0, 0), "P");
			else
				show_pos(p, ch_idx, x_pos, pos, QColor(255,0,0));
		}
	}

	if (_prop_p_peak_1 && _prop_p_peak_2 && _prop_p_type)
	{
		int pos1, pos2;

		pos1 = pos2 = 32767;

		if (ra_prop_get_value(_prop_p_peak_1, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			pos1 = ra_value_get_long(_vh);
		if (ra_prop_get_value(_prop_p_peak_2, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			pos2 = ra_value_get_long(_vh);

		long type = 0;
		if (ra_prop_get_value(_prop_p_type, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			type = ra_value_get_long(_vh);
		
		if (pos1 < 32767)
		{
			pos1 += qrs_pos;
			int x_pos = (int)((double)(pos1 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if ((type == 21) || (type == 212))
				below = false;
			if (in_edit_area)
				show_peak_edit(p, pos1, below);
			else
				show_peak(p, ch_idx, x_pos, pos1, below);
		}
		if (pos2 < 32767)
		{
			pos2 += qrs_pos;
			int x_pos = (int)((double)(pos2 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if (type == 121)
				below = false;
			if (in_edit_area)
				show_peak_edit(p, pos2, true);
			else
				show_peak(p, ch_idx, x_pos, pos2, true);
		}
	}

	if (ra_prop_get_value(_prop_p_end, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.p_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(p, pos, QColor(0, 0, 255), "P");
			else
				show_pos(p, ch_idx, x_pos, pos, QColor(0,0,255));
		}
	}
} // show_p_wave_info()


void
ecg_view_qt::show_qrs_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area)
{
	if (!_prop_qrs_start || !_prop_qrs_end || !_show_qrs_pos)
		return;

	if ((morph_flags & ECG_QRS_OK) == 0)
		return;

	if (ra_prop_get_value(_prop_qrs_start, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.qrs_start;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(p, pos, QColor(255, 0, 0), "QRS");
			else
				show_pos(p, ch_idx, x_pos, pos, QColor(255,0,0));
		}
	}
	if (ra_prop_get_value(_prop_qrs_end, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.qrs_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(p, pos, QColor(0, 0, 255), "QRS");
			else
				show_pos(p, ch_idx, x_pos, pos, QColor(0,0,255));
		}
	}

	if (_prop_q)
	{
		if (ra_prop_get_value(_prop_q, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(p, pos, true);
				else
					show_peak(p, ch_idx, x_pos, pos, true);
			}
		}
	}
	if (_prop_r)
	{
		if (ra_prop_get_value(_prop_r, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(p, pos, false);
				else
					show_peak(p, ch_idx, x_pos, pos, false);
			}
		}
	}
	if (_prop_s)
	{
		if (ra_prop_get_value(_prop_s, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(p, pos, true);
				else
					show_peak(p, ch_idx, x_pos, pos, true);
			}
		}
	}
	if (_prop_rs)
	{
		if (ra_prop_get_value(_prop_rs, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(p, pos, false);
				else
					show_peak(p, ch_idx, x_pos, pos, false);
			}
		}
	}
} // show_qrs_info()


void
ecg_view_qt::show_t_wave_info(QPainter *p, long ev_id, int ch_idx, int qrs_pos, long morph_flags, bool in_edit_area)
{
	if (!_prop_t_start || !_prop_t_end || !_show_t_wave_pos)
		return;

	if ((morph_flags & ECG_T_WAVE_OK) == 0)
		return;

	// often the position of T-wave start is wrong, therefore do not show it;
	// maybe later when the positions make more sense (or maybe it never)
// 	if (ra_prop_get_value(_prop_t_start, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
// 	{
// 		int pos = ra_value_get_long(_vh);
// 		if (in_edit_area)
// 			pos = _edit_beat.t_start;
// 		if (pos < 32767)
// 		{
// 			pos += qrs_pos;
// 			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
// 			if (in_edit_area)
// 				show_pos_edit(p, pos, QColor(255, 0, 255), "T");
// 			else
// 				show_pos(p, ch_idx, x_pos, pos, QColor(255, 0, 255));
// 		}
// 	}

	if (_prop_t_peak_1 && _prop_t_peak_2 && _prop_t_type)
	{
		int pos1, pos2;

		pos1 = pos2 = 32767;
		if (ra_prop_get_value(_prop_t_peak_1, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			pos1 = ra_value_get_long(_vh);
		if (ra_prop_get_value(_prop_t_peak_2, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			pos2 = ra_value_get_long(_vh);

		long type = 0;
		if (ra_prop_get_value(_prop_t_type, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
			type = ra_value_get_long(_vh);

		if (pos1 < 32767)
		{
			pos1 += qrs_pos;
			int x_pos = (int)((double)(pos1 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if ((type == 21) || (type == 212))
				below = false;
			if (in_edit_area)
				show_peak_edit(p, pos1, below);
			else
				show_peak(p, ch_idx, x_pos, pos1, below);
		}
		if (pos2 < 32767)
		{
			pos2 += qrs_pos;
			int x_pos = (int)((double)(pos2 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if (type == 121)
				below = false;
			if (in_edit_area)
				show_peak_edit(p, pos2, below);
			else
				show_peak(p, ch_idx, x_pos, pos2, below);
		}
	}

	if (ra_prop_get_value(_prop_t_end, ev_id, _ch[ch_idx].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.t_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(p, pos, QColor(0, 0, 255), "T");
			else
				show_pos(p, ch_idx, x_pos, pos, QColor(0,0,255));
		}
	}
} // show_t_wave_info()


void
ecg_view_qt::show_beat_rri(QPainter *p, long ev_id, long x_1, long x)
{
	if (!_prop_rri)
		return;

	ra_prop_get_value(_prop_rri, ev_id, -1, _vh);
	int rri = (int)ra_value_get_long(_vh);
	QString s;
	s.setNum(rri);
	int pos_rri = x_1 + (x - x_1) / 2;
	p->drawText(pos_rri - 30, 2, 60, 15, Qt::AlignHCenter, s);
} // show_beat_rri()


void
ecg_view_qt::show_beat_info(QPainter *p, long ev_id, long x)
{
	if (!_prop_qrs_temporal || !_prop_rri_ref || !_prop_rri_num_ref)
		return;

	ra_prop_get_value(_prop_qrs_temporal, ev_id, -1, _vh);
	int temp = (int)ra_value_get_long(_vh);
	ra_prop_get_value(_prop_rri_ref, ev_id, -1, _vh);
	int ref = (int)ra_value_get_long(_vh);
	ra_prop_get_value(_prop_rri_num_ref, ev_id, -1, _vh);
	int num_ref = (int)ra_value_get_long(_vh);

	char t[100];
	sprintf(t, "%d/%d/%d", temp, ref, num_ref);
	p->drawText(x - 30, 30, 60, 30, Qt::AlignHCenter, t);
} // show_beat_info()


void
ecg_view_qt::show_beat_index(QPainter *p, long ev_id, long x)
{
	/* show event-number below beat-classification (needed sometimes for debugging) */
	QString s;
	s = s.setNum(ev_id);
	p->drawText(x - 10, 15, 60, 30, Qt::AlignHCenter, s);
} // show_beat_index()


void
ecg_view_qt::show_pos(QPainter *p, int ch, int x, long pos, QColor col/* = QColor(0, 0, 0)*/)
{
	if (!_rh || (pos < 0))
		return;

	p->save();

	QPen pen;
	pen.setColor(col);
	p->setPen(pen);

	double max = draw_calc_max_value(ch);
	double val;
	long ret;
	if ((ret = ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val)) != 1)
	{
		p->restore();
		return;
	}
 	val -= _ch[ch].mean;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	p->drawLine(x, y + 10, x, y - 10);

	p->restore();
} // show_pos()


void
ecg_view_qt::show_pos_edit(QPainter *p, long pos, QColor col/*= QColor(0, 0, 0)*/,
			   const char *type/*=NULL*/, bool type_above/*=false*/)
{
	if (!_rh || (pos < 0) || (_edit_beat.data == NULL))
		return;

	int offset = pos - _edit_beat.start;
	if ((offset < 0) || (offset >= _edit_beat.num_data))
		return;

	int x_use = (int)((double)offset * _edit_beat.xscale);
	int y_use = (int)((_edit_beat.max - _edit_beat.data[offset]) * _edit_beat.yscale) + _edit_beat.draw_area_top;

	p->save();

	QPen pen;
	pen.setColor(col);
	p->setPen(pen);

	if (type_above)
		p->drawLine(x_use + 10, y_use - 50, x_use + 10, y_use + 10);
	else
		p->drawLine(x_use + 10, y_use + 50, x_use + 10, y_use - 10);

	if (type)
	{
		if (type_above)
			p->drawText(x_use, y_use - 60, type);
		else
			p->drawText(x_use, y_use + 70, type);
	}

	p->restore();
} // show_pos_edit()


void
ecg_view_qt::show_peak(QPainter *p, int ch, int x, long pos, bool below, QColor col/* = QColor(0, 0, 0)*/)
{
	if (!_rh || (pos < 0))
		return;

	p->save();

	QPen pen;
	pen.setColor(col);
	p->setPen(pen);

	double max = draw_calc_max_value(ch);
	double val;
	if (ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val) != 1)
	{
		p->restore();
		return;
	}
 	val -= _ch[ch].mean;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	int y_start = y - 10;
	if (below)
		y_start = y + 5;

	p->drawLine(x, y_start, x, y_start + 5);

	p->restore();
} // show_peak()


void
ecg_view_qt::show_peak_edit(QPainter *p, long pos, bool below, QColor col/* = QColor(0, 0, 0)*/)
{
	if (!_rh || (pos < 0))
		return;

	p->save();

	QPen pen;
	pen.setColor(col);
	p->setPen(pen);

	int offset = pos - _edit_beat.start;
	if ((offset < 0) || (offset >= _edit_beat.num_data))
	{
		p->restore();
		return;
	}

	int x_use = (int)((double)offset * _edit_beat.xscale);
	int y_use = (int)((_edit_beat.max - _edit_beat.data[offset]) * _edit_beat.yscale) + _edit_beat.draw_area_top;

	int y_start = y_use - 10;
	if (below)
		y_start = y_use + 5;

	p->drawLine(x_use + 10, y_start, x_use + 10, y_start + 5);

	p->restore();
} // show_peak_edit()


void
ecg_view_qt::show_events(QPainter *p)
{
	char **events = NULL;
	long *start = NULL;
	long *end = NULL;
	int n_events = 0;

	get_arr_events(&events, &start, &end, &n_events);
	if (n_events > 0)
	{
		for (int i = 0; i < n_events; i++)
		{
			int x, y;
			
			x = start[i];
			y = this->height() - 20;
			if (y != -1)
				p->drawText(x, y, events[i]);
		}
	}

	if (events)
	{
		for (int i = 0; i < n_events; i++)
		{
			if (events[i])
				free(events[i]);
		}
		free(events);
	}
	if (start)
		free(start);
	if (end)
		free(end);
} // show_events()


void
ecg_view_qt::plot_edit_area(QPainter *p)
{
	_edit_beat.draw_area = (_screen_height - _edit_beat.draw_area_bottom) - _edit_beat.draw_area_top;
	if (_edit_beat.draw_area < 1)
		_edit_beat.draw_area = 1;

	long mid_draw_area = _edit_beat.draw_area_top + (_edit_beat.draw_area/2);

	if (_edit_beat.event_id < 0)
	{
		p->drawText(10, mid_draw_area, QString(gettext("no beat selected")));
		return;
	}
	if (!_prop_morph_flags)
	{
		p->drawText(10, mid_draw_area, QString(gettext("beat morphology data is not available")));
		return;
	}
	if (_edit_beat.ch < 0)
	{
		p->drawText(10, mid_draw_area, QString(gettext("no specific channel was selected")));
		return;
	}

	p->save();

	QPoint *pts = new QPoint[_edit_beat.num_data];
	for (int i = 0; i < _edit_beat.num_data; i++)
	{
		int x = (int)((double)i * _edit_beat.xscale);
 		int y = (int)((_edit_beat.max - _edit_beat.data[i]) * _edit_beat.yscale);
		pts[i].setX(x + 10);
		pts[i].setY(y + _edit_beat.draw_area_top);
	}
	p->drawPolyline(pts, _edit_beat.num_data);

	if (_show_lowpass_edit_signal)
	{
		for (int i = 0; i < _edit_beat.num_data; i++)
		{
			int x = (int)((double)i * _edit_beat.xscale);
			int y = (int)((_edit_beat.max - _edit_beat.data_filt[i]) * _edit_beat.yscale);
			pts[i].setX(x + 10);
			pts[i].setY(y + _edit_beat.draw_area_top);
		}
		QPen pen;
		pen.setColor(QColor(255, 0, 0));
		p->setPen(pen);
		p->drawPolyline(pts, _edit_beat.num_data);
	}
	if (_show_highpass_edit_signal)
	{
		for (int i = 0; i < _edit_beat.num_data; i++)
		{
			int x = (int)((double)i * _edit_beat.xscale);
			int y = (int)((_edit_beat.max - _edit_beat.data_filt2[i]) * _edit_beat.yscale);
			pts[i].setX(x + 10);
			pts[i].setY(y + _edit_beat.draw_area_top);
		}
		QPen pen;
		pen.setColor(QColor(0, 0, 255));
		p->setPen(pen);
 		p->drawPolyline(pts, _edit_beat.num_data);
	}

	delete[] pts;
	p->restore();

	if (ra_prop_get_value(_prop_morph_flags, _edit_beat.event_id, _edit_beat.ch, _vh) != 0)
		return;

	_edit_beat.morph_flags = ra_value_get_long(_vh);

	if (!IS_ARTIFACT(_edit_beat.annot) && !IS_UNKNOWN(_edit_beat.annot))
	{
		if ((_edit_beat.annot != -1) && IS_SINUS(_edit_beat.annot))
			show_p_wave_info(p, _edit_beat.event_id, _edit_beat.ch_idx, _edit_beat.pos, _edit_beat.morph_flags, true);
		show_qrs_info(p, _edit_beat.event_id, _edit_beat.ch_idx, _edit_beat.pos, _edit_beat.morph_flags, true);
		show_t_wave_info(p, _edit_beat.event_id, _edit_beat.ch_idx, _edit_beat.pos, _edit_beat.morph_flags, true);
	}

	// showing the event-pos only for debug purposes
// 	show_pos_edit(p, _edit_beat.pos, QColor(255, 0, 0), "B");
	show_pos_edit(p, _edit_beat.fiducial_pos, QColor(0, 0, 0), "F", true);
	
	print_edit_beat_info(p);
} // plot_edit_area()


void
ecg_view_qt::print_edit_beat_info(QPainter *p)
{
	switch (_edit_beat.p_type)
	{
	case -1:
	case 0:
		_p_type_select->setCurrentIndex(0);
		break;
	case MORPH_TYPE_NEG:
		_p_type_select->setCurrentIndex(MORPH_TYPE_NEG_IDX);
		break;
	case MORPH_TYPE_POS:
		_p_type_select->setCurrentIndex(MORPH_TYPE_POS_IDX);
		break;
	case MORPH_TYPE_BI_NEG_POS:
		_p_type_select->setCurrentIndex(MORPH_TYPE_BI_NEG_POS_IDX);
		break;
	case MORPH_TYPE_BI_POS_NEG:
		_p_type_select->setCurrentIndex(MORPH_TYPE_BI_POS_NEG_IDX);
		break;
	default:
		_p_type_select->setCurrentIndex(5);
		break;
	}

	switch (_edit_beat.qrs_type)
	{
	case -1:
	case 0:
		_qrs_type_select->setCurrentIndex(0);
		break;
	case MORPH_TYPE_S:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_S_IDX);
		break;
	case MORPH_TYPE_R:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_R_IDX);
		break;
	case MORPH_TYPE_QR:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_QR_IDX);
		break;
	case MORPH_TYPE_RS:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_RS_IDX);
		break;
	case MORPH_TYPE_QRS:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_QRS_IDX);
		break;
	case MORPH_TYPE_RSR:
		_qrs_type_select->setCurrentIndex(MORPH_TYPE_RSR_IDX);
		break;
	default:
		_qrs_type_select->setCurrentIndex(7);
		break;
	}

	switch (_edit_beat.t_type)
	{
	case -1:
	case 0:
		_t_type_select->setCurrentIndex(0);
		break;
	case MORPH_TYPE_NEG:
		_t_type_select->setCurrentIndex(MORPH_TYPE_NEG_IDX);
		break;
	case MORPH_TYPE_POS:
		_t_type_select->setCurrentIndex(MORPH_TYPE_POS_IDX);
		break;
	case MORPH_TYPE_BI_NEG_POS:
		_t_type_select->setCurrentIndex(MORPH_TYPE_BI_NEG_POS_IDX);
		break;
	case MORPH_TYPE_BI_POS_NEG:
		_t_type_select->setCurrentIndex(MORPH_TYPE_BI_POS_NEG_IDX);
		break;
	default:
		_t_type_select->setCurrentIndex(5);
		break;
	}

	p->drawText(10, 25, "P-Wave");
	_p_type_select->move(120, 5);
	_p_type_select->show();

	p->drawText(10, 60, "QRS-Complex");
	_qrs_type_select->move(120, 40);
	_qrs_type_select->show();

	p->drawText(10, 95, "T-Wave");
	_t_type_select->move(120, 75);
	_t_type_select->show();

	_edit_apply->move(10, _screen_height - 50);
	_edit_apply->show();
	_edit_apply_all->move(120, _screen_height - 50);
	_edit_apply_all->show();
	_edit_close->move(EDIT_SPACE-110, _screen_height - 50);
	_edit_close->show();

	_edit_beat.draw_area = (_screen_height - _edit_beat.draw_area_bottom) - _edit_beat.draw_area_top;
	if (_edit_beat.draw_area < 1)
		_edit_beat.draw_area = 1;
} // print_edit_beat_info()


void
ecg_view_qt::mousePressEvent(QMouseEvent * ev)
{
	if (ev->modifiers() & Qt::ControlModifier)
	{
		mouseDoubleClickEvent(ev);
		return;
	}
	if (_in_edit_mode && (ev->button() == Qt::LeftButton) && (ev->x() < EDIT_SPACE))
	{
//		mouse_press_edit_area(ev->x(), ev->y());
		return;
	}

//	_popup_menu_ecg->setItemEnabled(_update_id, _process_waiting);

	if (ev->button() == Qt::LeftButton)
	{
		mouse_press(ev->x(), ev->y(), ev->modifiers() & Qt::ShiftModifier, false);
		ts_view_qt::mousePressEvent(ev);
		plot_cursor(NULL);

		if (!_in_edit_mode)
		{
 			if (_ev_idx < 0)
 				edit_mode_act->setEnabled(false);
 			else
 				edit_mode_act->setEnabled(true);
		}

	}
	else if (ev->button() == Qt::RightButton)
	{
		QAction *menu_act = NULL;
 		edit_mode_act->setChecked(_in_edit_mode);
		if (_in_edit_mode)
		{
			menu_act = _popup_menu_ecg->addMenu(_popup_menu_edit);
			menu_act->setText(QString("Edit Options"));
			show_lowpass_act->setChecked(_show_lowpass_edit_signal);
			show_highpass_act->setChecked(_show_highpass_edit_signal);
		}
		else
		{
			mouse_press(ev->x(), ev->y(), ev->modifiers() & Qt::ShiftModifier, false);
// 			if ((ev->x() < _left_space) && (ev->y() > _top_space))
// 			{
// 				id = _popup_menu->insertItem(QString(gettext("Edit morphology value's")),
// 							     _popmenu_edit_channel);
// 			}
// 			else if (ev->y() < _top_space)
// 			{
// 				id = _popup_menu->insertItem(QString(gettext("Edit morphology value's")),
// 							     _popmenu_edit_event);
// 			}
// 			else 
// 			{
// 				id = _popup_menu->insertItem(QString(gettext("Edit morphology value's")),
// 							     _popmenu_edit_single);
// 			}
			
			long idx = get_sel_pos(ev->x() - _left_space);
			if (idx != -1)
				set_event(NULL, _event_ids[idx]);
		}

		ts_view_qt::mousePressEvent(ev);

		if (menu_act)
 			_popup_menu_ecg->removeAction(menu_act);
	}
	else
		ts_view_qt::mousePressEvent(ev);		
} // mousePressEvent()


void
ecg_view_qt::plot_cursor(void *p)
{
	QPainter *p_use = (QPainter *)p;
	ts_view_qt::plot_cursor(p);

	if (_cursor_pos < _left_space)
		return;
	
	if ((_ev_idx != -1) && _prop_pos && _prop_class)
	{
		send_event();

		ra_prop_get_value(_prop_pos, _event_ids[_ev_idx], -1, _vh);
		long pos = ra_value_get_long(_vh);
		ra_prop_get_value(_prop_class, _event_ids[_ev_idx], -1, _vh);
		int cl = (int)ra_value_get_long(_vh);

		int x = (int) ((double) (pos - _curr_pos) * _max_xscale) + _left_space;

		if (!p)
			update();
		else
		{
			p_use->save();

			QFont f;
			// f.defaultFont();
			f.setBold(true);
			p_use->setFont(f);
			const char *c = raecg_get_class_string(cl);
			p_use->drawText(x - 10, 2, 20, 15, Qt::AlignHCenter, c);

			p_use->restore();
		}
	}
} // plot_cursor()


void
ecg_view_qt::slot_set_area_to_artifacts()
{
	set_area_to_artifacts();
} // slot_set_area_to_artifacts()


void
ecg_view_qt::slot_set_area_to_normals()
{
	set_area_to_normals();
} // slot_set_area_to_normals()


void
ecg_view_qt::slot_set_area_to_pacs()
{
	set_area_to_pacs();
} // slot_set_area_to_pacs()


void
ecg_view_qt::slot_set_area_to_pvcs()
{
	set_area_to_pvcs();
} // slot_set_area_to_pvcs()


void
ecg_view_qt::slot_set_area_to_paced()
{
	set_area_to_paced();
} // slot_set_area_to_paced()


void
ecg_view_qt::slot_remove_beats_in_area()
{
	remove_beats_in_area();
} // slot_remove_beats_in_area()


void
ecg_view_qt::sort_general(prop_handle ph, double value)
{
	ecg_view_general::sort_general(ph, value);
	repaint();
} // sort_general()

void
ecg_view_qt::sort_ch_found(int ch)
{
	ecg_view_general::sort_ch_found(ch);
	repaint();
} // sort_ch_found()


void
ecg_view_qt::sort_template(int templ_num)
{
	ecg_view_general::sort_template(templ_num);
	repaint();
} // sort_template()


void
ecg_view_qt::sort_rri(int short_first)
{
	ecg_view_general::sort_rri((bool)short_first, 0);
	repaint();
} // sort_rri()


void
ecg_view_qt::sort_class(int classification)
{
	ecg_view_general::sort_class(classification);
	repaint();
} // sort_class


void
ecg_view_qt::slot_toggle_edit_mode()
{
	toggle_edit_mode();
	edit_mode_act->setChecked(_in_edit_mode);
	update();
} // slot_toggle_edit_mode()


void
ecg_view_qt::slot_toggle_show_morph_pos()
{
	_show_morph_pos = !_show_morph_pos;
	show_morph_act->setChecked(_show_morph_pos);
	update();
} // slot_toggle_show_morph_pos()


void
ecg_view_qt::slot_toggle_show_p_wave_pos()
{
	_show_p_wave_pos = !_show_p_wave_pos;
	show_p_wave_act->setChecked(_show_p_wave_pos);
	update();
} // slot_toggle_show_p_wave_pos()


void
ecg_view_qt::slot_toggle_show_qrs_pos()
{
	_show_qrs_pos = !_show_qrs_pos;
	show_qrs_act->setChecked(_show_qrs_pos);
	update();
} // slot_toggle_show_qrs_pos()


void
ecg_view_qt::slot_toggle_show_t_wave_pos()
{
	_show_t_wave_pos = !_show_t_wave_pos;
	show_t_wave_act->setChecked(_show_t_wave_pos);
	update();
} // slot_toggle_show_t_wave_pos()


void
ecg_view_qt::slot_morphology_nok()
{
	if ((_curr_channel < 0) || (_ev_idx < 0))
		return;

	long mask = ECG_P_WAVE_OK | ECG_QRS_OK | ECG_T_WAVE_OK | ECG_U_WAVE_OK;
 	remove_single_morph_flags(~mask, _event_ids[_ev_idx], _ch[_curr_channel].ch_number);
} // slot_morphology_nok()


void
ecg_view_qt::slot_p_wave_nok()
{
	if ((_curr_channel < 0) || (_ev_idx < 0))
		return;

	remove_single_morph_flags(~ECG_P_WAVE_OK, _event_ids[_ev_idx], _ch[_curr_channel].ch_number);
} // slot_p_wave_nok()


void
ecg_view_qt::slot_qrs_nok()
{
	if ((_curr_channel < 0) || (_ev_idx < 0))
		return;

	remove_single_morph_flags(~ECG_QRS_OK, _event_ids[_ev_idx], _ch[_curr_channel].ch_number);
} // slot_qrs_nok()


void
ecg_view_qt::slot_t_wave_nok()
{
	if ((_curr_channel < 0) || (_ev_idx < 0))
		return;

	remove_single_morph_flags(~ECG_T_WAVE_OK, _event_ids[_ev_idx], _ch[_curr_channel].ch_number);
} // slot_t_wave_nok()


void
ecg_view_qt::slot_ev_morphology_nok()
{
	if (_ev_idx < 0)
		return;

	long mask = ECG_P_WAVE_OK | ECG_QRS_OK | ECG_T_WAVE_OK | ECG_U_WAVE_OK;
	remove_ev_morph_flags(~mask, _event_ids[_ev_idx]);
} // slot_ev_morphology_nok()


void
ecg_view_qt::slot_ev_p_wave_nok()
{
	if (_ev_idx < 0)
		return;

	remove_ev_morph_flags(~ECG_P_WAVE_OK, _event_ids[_ev_idx]);
} // slot_ev_p_wave_nok()


void
ecg_view_qt::slot_ev_qrs_nok()
{
	if (_ev_idx < 0)
		return;

	remove_ev_morph_flags(~ECG_QRS_OK, _event_ids[_ev_idx]);
} // slot_ev_qrs_nok()


void
ecg_view_qt::slot_ev_t_wave_nok()
{
	if (_ev_idx < 0)
		return;

	remove_ev_morph_flags(~ECG_T_WAVE_OK, _event_ids[_ev_idx]);
} // slot_ev_t_wave_nok()


void
ecg_view_qt::slot_ch_morphology_nok()
{
	if (_curr_channel < 0)
		return;

	long mask = ECG_P_WAVE_OK | ECG_QRS_OK | ECG_T_WAVE_OK | ECG_U_WAVE_OK;
	remove_ch_morph_flags(~mask, _ch[_curr_channel].ch_number);
} // slot_ch_morphology_nok()


void
ecg_view_qt::slot_ch_p_wave_nok()
{
	if (_curr_channel < 0)
		return;

	remove_ch_morph_flags(~ECG_P_WAVE_OK, _ch[_curr_channel].ch_number);
} // slot_ch_p_wave_nok()


void
ecg_view_qt::slot_ch_qrs_nok()
{
	if (_curr_channel < 0)
		return;

	remove_ch_morph_flags(~ECG_QRS_OK, _ch[_curr_channel].ch_number);
} // slot_ch_qrs_nok()


void
ecg_view_qt::slot_ch_t_wave_nok()
{
	if (_curr_channel < 0)
		return;

	remove_ch_morph_flags(~ECG_T_WAVE_OK, _ch[_curr_channel].ch_number);
} // slot_ch_t_wave_nok()


void
ecg_view_qt::slot_p_type_changed(int idx)
{
	long force_type = 0;
	switch(idx)
	{
	case MORPH_TYPE_NEG_IDX:
		force_type = MORPH_TYPE_NEG;
		break;
	case MORPH_TYPE_POS_IDX:
		force_type = MORPH_TYPE_POS;
		break;
	case MORPH_TYPE_BI_NEG_POS_IDX:
		force_type = MORPH_TYPE_BI_NEG_POS;
		break;
	case MORPH_TYPE_BI_POS_NEG_IDX:
		force_type = MORPH_TYPE_BI_POS_NEG;
		break;
	}

	redo_wave_boundaries(force_type, 0, 0, false, false, NULL);

	update();
	setFocus();

	_edit_apply->setEnabled(true);
	_edit_apply_all->setEnabled(true);
} // slot_p_type_changed()


void
ecg_view_qt::slot_qrs_type_changed(int idx)
{
	long force_type = 0;
	switch(idx)
	{
	case MORPH_TYPE_S_IDX:
		force_type = MORPH_TYPE_S;
		break;
	case MORPH_TYPE_R_IDX:
		force_type = MORPH_TYPE_R;
		break;
	case MORPH_TYPE_QR_IDX:
		force_type = MORPH_TYPE_QR;
		break;
	case MORPH_TYPE_RS_IDX:
		force_type = MORPH_TYPE_RS;
		break;
	case MORPH_TYPE_QRS_IDX:
		force_type = MORPH_TYPE_QRS;
		break;
	case MORPH_TYPE_RSR_IDX:
		force_type = MORPH_TYPE_RSR;
		break;
	}

	redo_wave_boundaries(0, force_type, 0, false, false, NULL);

	update();
	setFocus();

	_edit_apply->setEnabled(true);
	_edit_apply_all->setEnabled(true);
} // slot_qrs_type_changed()


void
ecg_view_qt::slot_t_type_changed(int idx)
{
	long force_type = 0;
	switch(idx)
	{
	case MORPH_TYPE_NEG_IDX:
		force_type = MORPH_TYPE_NEG;
		break;
	case MORPH_TYPE_POS_IDX:
		force_type = MORPH_TYPE_POS;
		break;
	case MORPH_TYPE_BI_NEG_POS_IDX:
		force_type = MORPH_TYPE_BI_NEG_POS;
		break;
	case MORPH_TYPE_BI_POS_NEG_IDX:
		force_type = MORPH_TYPE_BI_POS_NEG;
		break;
	}

	redo_wave_boundaries(0, 0, force_type, false, false, NULL);

	update();
	setFocus();

	_edit_apply->setEnabled(true);
	_edit_apply_all->setEnabled(true);
} // slot_t_type_changed()


void
ecg_view_qt::slot_edit_apply()
{
	long p_type = 0;
	if (_edit_beat.p_type_save != _edit_beat.p_type)
		p_type = _edit_beat.p_type;

	long qrs_type = 0;
	if (_edit_beat.qrs_type_save != _edit_beat.qrs_type)
		qrs_type = _edit_beat.qrs_type;

	long t_type = 0;
	if (_edit_beat.t_type_save != _edit_beat.t_type)
		t_type = _edit_beat.t_type;

// 	if ((p_type == 0) && (qrs_type == 0) && (t_type == 0))
// 		return;

	redo_wave_boundaries(p_type, qrs_type, t_type, true, false, NULL);
	fill_edit_beat_data();

	update();
	setFocus();

	_edit_apply->setEnabled(false);
	_edit_apply_all->setEnabled(false);
} // slot_edit_apply()


void
ecg_view_qt::slot_edit_apply_all()
{
	long p_type = 0;
	if (_edit_beat.p_type_save != _edit_beat.p_type)
		p_type = _edit_beat.p_type;

	long qrs_type = 0;
	if (_edit_beat.qrs_type_save != _edit_beat.qrs_type)
		qrs_type = _edit_beat.qrs_type;

	long t_type = 0;
	if (_edit_beat.t_type_save != _edit_beat.t_type)
		t_type = _edit_beat.t_type;

	redo_wave_boundaries(p_type, qrs_type, t_type, true, true, NULL);
	fill_edit_beat_data();

	update();
	setFocus();

	_edit_apply->setEnabled(false);
	_edit_apply_all->setEnabled(false);
} // slot_edit_apply_all()


void
ecg_view_qt::slot_show_low_pass_signal()
{
	_show_lowpass_edit_signal = (_show_lowpass_edit_signal ? false : true);
	show_lowpass_act->setChecked(_show_lowpass_edit_signal);
	update();
} // slot_show_low_pass_signal()


void
ecg_view_qt::slot_show_high_pass_signal()
{
	_show_highpass_edit_signal = (_show_highpass_edit_signal ? false : true);
 	show_highpass_act->setChecked(_show_highpass_edit_signal);
	update();
} // slot_show_high_pass_signal()


void
ecg_view_qt::resizeEvent(QResizeEvent *ev)
{
	_screen_height = ev->size().height();
	if (_in_edit_mode)
	{
		long idx_save = _ev_idx;
		_ev_idx = _edit_beat.event_idx;
		fill_edit_beat_data();
		_ev_idx = idx_save;
	}
} // resizeEvent()


void
ecg_view_qt::slot_edit_close()
{
	_edit_close->setDown(false);
	toggle_edit_mode();
	update();
} // slot_edit_close()


void
ecg_view_qt::mouseDoubleClickEvent(QMouseEvent *ev)
{
	// first check if an annotation should be edited
	_dbl_left_click_handled = false;
	ts_view_qt::mouseDoubleClickEvent(ev);
	if (_dbl_left_click_handled)
		return;		// an annotation was processed, end function

	if (ev->button() != Qt::LeftButton)
		return;

	mouse_press(ev->x(), ev->y(), ev->modifiers() & Qt::ShiftModifier, true);
	if (_ev_idx < 0)
		return;

	if (!_in_edit_mode)
		toggle_edit_mode();
	update();
} // mouseDoubleClickEvent()


void
ecg_view_qt::keyPressEvent(QKeyEvent * ev)
{
	if (!_in_edit_mode)
		ts_view_qt::keyPressEvent(ev);
} // keyPressEvent()

