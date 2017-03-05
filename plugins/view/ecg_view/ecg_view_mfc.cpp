/*----------------------------------------------------------------------------
 * ecg_view_mfc.cpp
 *
 * 
 *
 * Author(s): Raphael Schneider (rasch@med1.med.tum.de)
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <ctype.h>

#include <ra_ecg.h>
#include <ra_detect.h>

#include "ecg_view_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ra_view_ts

//IMPLEMENT_DYNCREATE(ecg_view_mfc, ts_view_mfc )
BEGIN_MESSAGE_MAP(ecg_view_mfc, ts_view_mfc)
	//{{AFX_MSG_MAP(ecg_view_mfc)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_COMMAND(IDM_SHORT_RRI_OPTIONS, _sort_short_rri)
	ON_COMMAND(IDM_LONG_RRI_OPTIONS, _sort_long_rri)
	ON_COMMAND(IDM_TOGGLE_EDIT_MODE, _toggle_edit_mode_menu)
	ON_COMMAND(ECG_NORMAL_BEAT, _sort_normal)
	ON_COMMAND(ECG_VPC_BEAT, _sort_vpc)
	ON_COMMAND(ECG_PACED_BEAT, _sort_paced)
	ON_COMMAND(ECG_ARTIFACT_BEAT, _sort_artifact)
	ON_COMMAND_RANGE(ECG_VIEW_CH_START, ECG_VIEW_CH_MAX, _sort_ch_use)
	ON_COMMAND_RANGE(ECG_VIEW_TEMPLATE_START, ECG_VIEW_TEMPLATE_MAX, _sort_template)
	ON_CBN_SELCHANGE(ID_P_TYPE_SEL, _p_type_changed)
	ON_CBN_SELCHANGE(ID_QRS_TYPE_SEL, _qrs_type_changed)
	ON_CBN_SELCHANGE(ID_T_TYPE_SEL, _t_type_changed)
	ON_BN_CLICKED(ID_APPLY_MORPH_CHANGE, _edit_apply_pressed)
	ON_BN_CLICKED(ID_APPLY_ALL_MORPH_CHANGE, _edit_apply_all_pressed)
	ON_BN_CLICKED(ID_EDIT_CLOSE, _edit_close_pressed)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard-Druckbefehle
END_MESSAGE_MAP()

ecg_view_mfc::ecg_view_mfc() : ts_view_mfc(), ecg_view_general(NULL), ts_view_general(NULL)
{
}


ecg_view_mfc::~ecg_view_mfc()
{
}				// destructor


void
ecg_view_mfc::update()
{
	Invalidate();
}  // update()


void
ecg_view_mfc::init(meas_handle mh, plugin_handle pl, int num_ch, int *ch)
{
	ts_view_mfc::init(mh, pl, num_ch, ch);
	ecg_view_general::init();

	init_menu();
}  // init()


void
ecg_view_mfc::init_menu()
{
	_popup_menu_sort.CreatePopupMenu();
	for (int i = 1; i <= _num_ch; i++)
	{
		char t[100];
		sprintf(t, gettext("beats found in %d channel%s"), i, (i > 1 ? "s" : ""));
		_popup_menu_sort.AppendMenu(MF_STRING, ECG_VIEW_CH_START + (i-1), t);

		// if no event-property with the channel information is not
		// available, dim the entries
		if (_prop_ch == NULL)
			_popup_menu_sort.EnableMenuItem(ECG_VIEW_CH_START + (i-1), MF_GRAYED | MF_BYCOMMAND);
	}
	_popup_menu_sort.AppendMenu(MF_SEPARATOR, 0, "");

	_popup_menu_sort.AppendMenu(MF_STRING, IDM_SHORT_RRI_OPTIONS, gettext("shortest rr-interval first"));
	_popup_menu_sort.AppendMenu(MF_STRING, IDM_LONG_RRI_OPTIONS, gettext("longest rr-interval first"));
	_popup_menu_sort.AppendMenu(MF_SEPARATOR, 0, "");

	_popup_menu_sort.AppendMenu(MF_STRING, ECG_NORMAL_BEAT, gettext("normal beats"));
	_popup_menu_sort.AppendMenu(MF_STRING, ECG_VPC_BEAT, gettext("VPCs"));
	_popup_menu_sort.AppendMenu(MF_STRING, ECG_PACED_BEAT, gettext("paced beats"));
	_popup_menu_sort.AppendMenu(MF_STRING, ECG_ARTIFACT_BEAT, gettext("Artifacts"));

	if (_num_templates > 0)
	{
		_popup_menu_template.CreatePopupMenu();
		for (int i = 0; i < _num_templates; i++)
		{
			char t[100];
			sprintf(t, gettext("beats in template %d"), i);
			_popup_menu_template.AppendMenu(MF_STRING, ECG_VIEW_TEMPLATE_START + i, t);
		}
	}

	_popup_menu_ecg.CreatePopupMenu();
	_popup_menu_ecg.AppendMenu(MF_STRING, IDM_TOGGLE_EDIT_MODE, gettext("Edit Mode"));
	// at the beginning no event is selected, therefore switching to edit mode
	// makes no sense
	_popup_menu_ecg.EnableMenuItem(IDM_TOGGLE_EDIT_MODE, MF_GRAYED);

	_popup_menu_ecg.AppendMenu(MF_POPUP, (UINT)_popup_menu_sort.GetSafeHmenu(), gettext("sort"));

	if (_num_templates > 0)
		_popup_menu_ecg.AppendMenu(MF_POPUP, (UINT)_popup_menu_template.GetSafeHmenu(), gettext("templates"));

	_popup_menu.AppendMenu(MF_POPUP, (UINT)_popup_menu_ecg.GetSafeHmenu(), gettext("ecg"));
}  // init_menu()


void
ecg_view_mfc::init_edit()
{
	_p_type_select.Create(WS_CHILD|WS_VSCROLL|CBS_DROPDOWNLIST, 
							CRect(120, 5, 300, 200), this, ID_P_TYPE_SEL);
	_p_type_select.AddString(gettext("negative"));
	_p_type_select.AddString(gettext("positive"));
	_p_type_select.AddString(gettext("bi-phasic (neg/pos)"));
	_p_type_select.AddString(gettext("bi-phasic (pos/neg)"));
	_p_type_select.AddString(gettext("unknown"));

	_qrs_type_select.Create(WS_CHILD|WS_VSCROLL|CBS_DROPDOWNLIST, 
							CRect(120, 40, 300, 200), this, ID_QRS_TYPE_SEL);
	_qrs_type_select.AddString(gettext("S"));
	_qrs_type_select.AddString(gettext("R"));
	_qrs_type_select.AddString(gettext("QR"));
	_qrs_type_select.AddString(gettext("RS"));
	_qrs_type_select.AddString(gettext("QRS"));
	_qrs_type_select.AddString(gettext("RSR'"));
	_qrs_type_select.AddString(gettext("unknown"));

	_t_type_select.Create(WS_CHILD|WS_VSCROLL|CBS_DROPDOWNLIST, 
							CRect(120, 75, 300, 200), this, ID_T_TYPE_SEL);
	_t_type_select.AddString(gettext("negative"));
	_t_type_select.AddString(gettext("positive"));
	_t_type_select.AddString(gettext("bi-phasic (neg/pos)"));
	_t_type_select.AddString(gettext("bi-phasic (pos/neg)"));
	_t_type_select.AddString(gettext("unknown"));

	_edit_apply.Create(gettext("Apply"), WS_CHILD|BS_PUSHBUTTON,
						CRect(10, _screen_height - 50, 100, 30),
						this, ID_APPLY_MORPH_CHANGE);
	_edit_apply_all.Create(gettext("Apply all"), WS_CHILD|BS_PUSHBUTTON,
						CRect(120, _screen_height - 50, 100, 30),
						this, ID_APPLY_ALL_MORPH_CHANGE);
	_edit_close.Create(gettext("Close"), WS_CHILD|BS_PUSHBUTTON,
						CRect(EDIT_SPACE - 110, _screen_height - 50, 100, 30),
						this, ID_EDIT_CLOSE);
}  // init_edit()


void
ecg_view_mfc::do_update()
{
	get_eventtypes();
}  // do_update()


void
ecg_view_mfc::OnPaint()
{
	ts_view_mfc::OnPaint();

	CDC *dc = GetDC();
	dc->SelectObject(_font);
	plot_beats(dc);

	// status bar
	if (_ts_opt.show_status && _num_sort)
	{
		char t[100];
		sprintf(t, "%ld / %ld", (_curr_sort + 1), _num_sort);
		UINT ta_save = dc->SetTextAlign(TA_RIGHT | TA_BASELINE);
		dc->TextOut(_screen_width - 4, _screen_height - 5, t, strlen(t));
		dc->SetTextAlign(ta_save);
	}

	plot_cursor(dc, _cursor_ch);

	if (_in_edit_mode)
		plot_edit_area(dc);
	else
	{
		_p_type_select.ShowWindow(SW_HIDE);
		_qrs_type_select.ShowWindow(SW_HIDE);
		_t_type_select.ShowWindow(SW_HIDE);
		_edit_apply.ShowWindow(SW_HIDE);
		_edit_apply_all.ShowWindow(SW_HIDE);
		_edit_close.ShowWindow(SW_HIDE);

		_edit_apply.EnableWindow(false);
		_edit_apply_all.EnableWindow(false);
	}
}  // OnPaint()


void
ecg_view_mfc::plot_beats(CDC * dc)
{
	if (_num_events == 0)
		return;

	int *x, num;
	long *pos;
	get_beat_pos(&x, &pos, &num);

	int w, h;
	get_text_extent("xX", &w, &h);
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
		if (pos)
			delete[]pos;
		return;
	}

	UINT ta_save = dc->SetTextAlign(TA_CENTER | TA_BASELINE);
	dc->SetBkMode(TRANSPARENT);
	for (i = 0; i < num; i++)
	{
		show_beat_class(dc, pos[i], x[i]);

		// if fast speed -> show only annotation
		if (d < (3 * w))
			continue;

		show_beat_template(dc, pos[i], x[i]);
		show_beat_position(dc, pos[i], x[i]);
		
		if (i > 0)
			show_beat_rri(dc, pos[i], x[i - 1], x[i]);
			
#if SHOW_BEAT_DEBUG_INFO
		show_beat_info(dc, pos[i], x[i]);
		show_beat_index(dc, pos[i], x[i]+20);
#endif // SHOW_BEAT_DEBUG_INFO
	}

	dc->SetTextAlign(ta_save);

	if (x)
		delete[]x;
	if (pos)
		delete[]pos;
} // plot_beats()


void
ecg_view_mfc::show_beat_class(CDC *dc, long beat, int x)
{
	if (!_prop_class)
		return;

	// show beat classification
	ra_prop_get_value(_prop_class, beat, -1, _vh);
	int cl = (int)ra_value_get_long(_vh);
	char *c = raecg_get_class_string(cl);
	dc->TextOut(x, 13, c, strlen(c));
} // show_beat_class()


void
ecg_view_mfc::show_beat_template(CDC *dc, long beat, int x)
{
	if (!_prop_template)
		return;

	ra_prop_get_value(_prop_template, beat, -1, _vh);
	int templ = (int)ra_value_get_long(_vh);
	char t[40];
	itoa(templ, t, 10);

	int x_templ = x;
	if (_prop_template_corr && (templ >= 0))
	{
		ra_prop_get_value(_prop_template_corr, beat, -1, _vh);
		double corr = ra_value_get_double(_vh);

		char t2[20];
		sprintf(t2, " (%.2f)", corr);
		strcat(t, t2);
		x_templ -= 4;
	}

	dc->TextOut(x_templ, 28, t, strlen(t));
}  // show_beat_template()


void
ecg_view_mfc::show_beat_position(CDC *dc, long beat, int x)
{
	if (!_prop_pos)
		return;

	int cl = -1;
	if (_prop_class)
	{
		ra_prop_get_value(_prop_class, beat, -1, _vh);
		cl = (int)ra_value_get_long(_vh);
	}

	// show positions
	for (int j = 0; j < _num_ch; j++)
	{
		if (!_ch[j].show_it || (_ch[j].type != RA_CH_TYPE_ECG))
			continue;

		if (_prop_ch)
		{
			long curr = 1L << _ch[j].ch_number;
			ra_prop_get_value(_prop_ch, beat, -1, _vh);
			long ch_found = ra_value_get_long(_vh);
			if (!(ch_found & curr))
				continue;
		}

		ra_prop_get_value(_prop_pos, beat, -1, _vh);
		int qrs_pos = ra_value_get_long(_vh);
// 		show_pos(dc, j, x, qrs_pos);

		if (!_prop_morph_flags)
			continue;
		if (ra_prop_get_value(_prop_morph_flags, beat, _ch[j].ch_number, _vh) != 0)
			continue;
		long morph_flags = ra_value_get_long(_vh);

		if (!IS_ARTIFACT(cl) && !IS_UNKNOWN(cl))
		{
			if ((cl != -1) && IS_SINUS(cl))
				show_p_wave_info(dc, beat, j, qrs_pos, morph_flags, false);
			show_qrs_info(dc,beat, j, qrs_pos, morph_flags, false);
			show_t_wave_info(dc, beat, j, qrs_pos, morph_flags, false);
		}
		else
			show_pos(dc, j, x, qrs_pos);
	}
}  // show_beat_position()


void
ecg_view_mfc::show_p_wave_info(CDC *dc, long ev_id, int ch, int qrs_pos,
							   long morph_flags, bool in_edit_area)
{
	if (!_prop_p_start || !_prop_p_end || !_show_p_wave_pos)
		return;

	if ((morph_flags & ECG_P_WAVE_OK) == 0)
		return;

	if (ra_prop_get_value(_prop_p_start, ev_id, _ch[ch].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.p_start;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(dc, ch, x_pos, pos, RGB(255, 0, 0), "P");
			else
				show_pos(dc, ch, x_pos, pos, RGB(255,0,0));
		}
	}

	if (_prop_p_peak_1 && _prop_p_peak_2 && _prop_p_type)
	{
		int pos1, pos2;

		pos1 = pos2 = 32767;

		if (ra_prop_get_value(_prop_p_peak_1, ev_id, _ch[ch].ch_number, _vh) == 0)
			pos1 = ra_value_get_long(_vh);
		if (ra_prop_get_value(_prop_p_peak_2, ev_id, _ch[ch].ch_number, _vh) == 0)
			pos2 = ra_value_get_long(_vh);

		long type = 0;
		if (ra_prop_get_value(_prop_p_type, ev_id, _ch[ch].ch_number, _vh) == 0)
			type = ra_value_get_long(_vh);
		
		if (pos1 < 32767)
		{
			pos1 += qrs_pos;
			int x_pos = (int)((double)(pos1 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if ((type == 21) || (type == 212))
				below = false;
			if (in_edit_area)
				show_peak_edit(dc, ch, x_pos, pos1, below);
			else
				show_peak(dc, ch, x_pos, pos1, below);
		}
		if (pos2 < 32767)
		{
			pos2 += qrs_pos;
			int x_pos = (int)((double)(pos2 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if (type == 121)
				below = false;
			if (in_edit_area)
				show_peak_edit(dc, ch, x_pos, pos2, true);
			else
				show_peak(dc, ch, x_pos, pos2, true);
		}
	}

	if (ra_prop_get_value(_prop_p_end, ev_id, _ch[ch].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.p_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(dc, ch, x_pos, pos, RGB(0, 0, 255), "P");
			else
				show_pos(dc, ch, x_pos, pos, RGB(0,0,255));
		}
	}
} // show_p_wave_info()


void
ecg_view_mfc::show_qrs_info(CDC* dc, long ev_id, int ch, int qrs_pos, long morph_flags,
								bool in_edit_area)
{
	if (!_prop_qrs_start || !_prop_qrs_end || !_show_qrs_pos)
		return;

	if ((morph_flags & ECG_QRS_OK) == 0)
		return;

	if (ra_prop_get_value(_prop_qrs_start, ev_id, _ch[ch].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.qrs_start;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(dc, ch, x_pos, pos, RGB(255, 0, 0), "QRS");
			else
				show_pos(dc, ch, x_pos, pos, RGB(255,0,0));
		}
	}
	if (ra_prop_get_value(_prop_qrs_end, ev_id, _ch[ch].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.qrs_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(dc, ch, x_pos, pos, RGB(0, 0, 255), "QRS");
			else
				show_pos(dc, ch, x_pos, pos, RGB(0,0,255));
		}
	}

	if (_prop_q)
	{
		if (ra_prop_get_value(_prop_q, ev_id, _ch[ch].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(dc, ch, x_pos, pos, true);
				else
					show_peak(dc, ch, x_pos, pos, true);
			}
		}
	}
	if (_prop_r)
	{
		if (ra_prop_get_value(_prop_r, ev_id, _ch[ch].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(dc, ch, x_pos, pos, false);
				else
					show_peak(dc, ch, x_pos, pos, false);
			}
		}
	}
	if (_prop_s)
	{
		if (ra_prop_get_value(_prop_s, ev_id, _ch[ch].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(dc, ch, x_pos, pos, true);
				else
					show_peak(dc, ch, x_pos, pos, true);
			}
		}
	}
	if (_prop_rs)
	{
		if (ra_prop_get_value(_prop_rs, ev_id, _ch[ch].ch_number, _vh) == 0)
		{
			int pos = ra_value_get_long(_vh);
			if (pos < 32767)
			{
				pos += qrs_pos;
				int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
				if (in_edit_area)
					show_peak_edit(dc, ch, x_pos, pos, false);
				else
					show_peak(dc, ch, x_pos, pos, false);
			}
		}
	}
} // show_qrs_info()


void
ecg_view_mfc::show_t_wave_info(CDC *dc, long ev_id, int ch, int qrs_pos, long morph_flags, bool in_edit_area)
{
	if (!_prop_t_start || !_prop_t_end || !_show_t_wave_pos)
		return;

	if ((morph_flags & ECG_T_WAVE_OK) == 0)
		return;

	// often the position of T-wave start is wrong, therefore do not show it;
	// maybe later when the positions make more sense (or maybe it never)
// 	if (ra_prop_get_value(_prop_t_start, ev_id, _ch[ch].ch_number, _vh) == 0)
// 	{
// 		int pos = ra_value_get_long(_vh);
// 		if (in_edit_area)
// 			pos = _edit_beat.t_start;
// 		if (pos < 32767)
// 		{
// 			pos += qrs_pos;
// 			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
// 			if (in_edit_area)
// 				show_pos_edit(dc, ch, x_pos, pos, RGB(255, 0, 255), "T");
// 			else
// 				show_pos(dc, ch, x_pos, pos, RGB(255, 0, 255));
// 		}
// 	}

	if (_prop_t_peak_1 && _prop_t_peak_2 && _prop_t_type)
	{
		int pos1, pos2;

		pos1 = pos2 = 32767;
		if (ra_prop_get_value(_prop_t_peak_1, ev_id, _ch[ch].ch_number, _vh) == 0)
			pos1 = ra_value_get_long(_vh);
		if (ra_prop_get_value(_prop_t_peak_2, ev_id, _ch[ch].ch_number, _vh) == 0)
			pos2 = ra_value_get_long(_vh);

		long type = 0;
		if (ra_prop_get_value(_prop_t_type, ev_id, _ch[ch].ch_number, _vh) == 0)
			type = ra_value_get_long(_vh);

		if (pos1 < 32767)
		{
			pos1 += qrs_pos;
			int x_pos = (int)((double)(pos1 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if ((type == 21) || (type == 212))
				below = false;
			if (in_edit_area)
				show_peak_edit(dc, ch, x_pos, pos1, below);
			else
				show_peak(dc, ch, x_pos, pos1, below);
		}
		if (pos2 < 32767)
		{
			pos2 += qrs_pos;
			int x_pos = (int)((double)(pos2 - _curr_pos) * _max_xscale) + _left_space;
			bool below = true;
			if (type == 121)
				below = false;
			if (in_edit_area)
				show_peak_edit(dc, ch, x_pos, pos2, below);
			else
				show_peak(dc, ch, x_pos, pos2, below);
		}
	}

	if (ra_prop_get_value(_prop_t_end, ev_id, _ch[ch].ch_number, _vh) == 0)
	{
		int pos = ra_value_get_long(_vh);
		if (in_edit_area)
			pos = _edit_beat.t_end;
		if (pos < 32767)
		{
			pos += qrs_pos;
			int x_pos = (int)((double)(pos - _curr_pos) * _max_xscale) + _left_space;
			if (in_edit_area)
				show_pos_edit(dc, ch, x_pos, pos, RGB(0, 0, 255), "T");
			else
				show_pos(dc, ch, x_pos, pos, RGB(0,0,255));
		}
	}
} // show_t_wave_info()


void
ecg_view_mfc::show_beat_rri(CDC *dc, long beat, int x_1, int x)
{
	if (!_prop_rri)
		return;

	int xrri = x_1 + (x - x_1) / 2;
	ra_prop_get_value(_prop_rri, beat, -1, _vh);
	int rri = (int)ra_value_get_long(_vh);
	char t[21];
	itoa(rri, t, 10);
	dc->TextOut(xrri, 15, t, strlen(t));
}  // show_beat_rri()


void
ecg_view_mfc::show_beat_info(CDC *dc, long beat, long x)
{
	if (!_prop_qrs_temporal || !_prop_rri_ref || !_prop_rri_num_ref)
		return;

	ra_prop_get_value(_prop_qrs_temporal, beat, -1, _vh);
	int temp = (int)ra_value_get_long(_vh);
	ra_prop_get_value(_prop_rri_ref, beat, -1, _vh);
	int ref = (int)ra_value_get_long(_vh);
	ra_prop_get_value(_prop_rri_num_ref, beat, -1, _vh);
	int num_ref = (int)ra_value_get_long(_vh);

	char t[100];
	sprintf(t, "%d/%d/%d", temp, ref, num_ref);
	dc->TextOut(x, 43, t, strlen(t));
} // show_beat_info()


void
ecg_view_mfc::show_beat_index(CDC *dc, long beat, long x)
{
	/* show indexnumber below beat-classification (needed sometimes for debugging) */
	char t[21];
	itoa((int)beat, t, 10);
	dc->TextOut(x, 20, t, strlen(t));
} // show_beat_index()


void
ecg_view_mfc::show_pos(CDC * dc, int ch, int x, long pos, COLORREF col/* = RGB(0,0,0)*/)
{
	double max = draw_calc_max_value(ch);
	double val;
	if (ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val) != 1)
		return;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	CPen *pen = new CPen;
	pen->CreatePen(PS_SOLID, 1, col);
	CPen *pen_save = (CPen *)dc->SelectObject(pen);

	dc->MoveTo(x, y + 10);
	dc->LineTo(x, y - 10);

	dc->SelectObject(pen_save);
	pen->DeleteObject();
	delete pen;
}  // show_pos()


void
ecg_view_mfc::show_pos_edit(CDC *dc, int ch, int x, long pos,
							COLORREF col/* = RGB(0,0,0)*/, char *type/* = NULL */)
{
	if (!_rh || (pos < 0) || (_edit_beat.data == NULL))
		return;

	int offset = pos - _edit_beat.start;
	if ((offset < 0) || (offset >= _edit_beat.num_data))
		return;

	int x_use = (int)((double)offset * _edit_beat.xscale);
	int y_use = (int)((_edit_beat.max - _edit_beat.data[offset]) * _edit_beat.yscale) + EDIT_SPACE_VERT;

	CPen *pen = new CPen;
	pen->CreatePen(PS_SOLID, 1, col);
	CPen *pen_save = (CPen *)dc->SelectObject(pen);

	dc->MoveTo(x_use + 10, y_use + 50);
	dc->LineTo(x_use + 10, y_use - 10);

	if (type)
		dc->TextOut(x_use, y_use + 70, type, strlen(type));

	dc->SelectObject(pen_save);
	pen->DeleteObject();
	delete pen;
} // show_pos_edit()


void
ecg_view_mfc::show_peak(CDC *dc, int ch, int x, long pos, bool below, COLORREF col/* = RGB(0, 0, 0)*/)
{
	if (!_rh || (pos < 0))
		return;

	double max = draw_calc_max_value(ch);
	double val;
	if (ra_raw_get_unit(_rh, _ch[ch].ch_number, pos, 1, &val) != 1)
		return;
 	val -= _ch[ch].mean;

	int amp = draw_calc_amplitude(max, val, ch);
	int y = amp + _top_space + _ch[ch].top_win_pos;

	int y_start = y - 10;
	if (below)
		y_start = y + 5;

	CPen *pen = new CPen;
	pen->CreatePen(PS_SOLID, 1, col);
	CPen *pen_save = (CPen *)dc->SelectObject(pen);

	dc->MoveTo(x, y_start);
	dc->LineTo(x, y_start + 5);

	dc->SelectObject(pen_save);
	pen->DeleteObject();
	delete pen;
} // show_peak()


void
ecg_view_mfc::show_peak_edit(CDC *dc, int ch, int x, long pos, bool below, COLORREF col/* = RGB(0, 0, 0)*/)
{
	if (!_rh || (pos < 0))
		return;

	int offset = pos - _edit_beat.start;
	if ((offset < 0) || (offset >= _edit_beat.num_data))
		return;

	int x_use = (int)((double)offset * _edit_beat.xscale);
	int y_use = (int)((_edit_beat.max - _edit_beat.data[offset]) * _edit_beat.yscale) + EDIT_SPACE_VERT;

	int y_start = y_use - 10;
	if (below)
		y_start = y_use + 5;

	CPen *pen = new CPen;
	pen->CreatePen(PS_SOLID, 1, col);
	CPen *pen_save = (CPen *)dc->SelectObject(pen);

	dc->MoveTo(x_use + 10, y_start);
	dc->LineTo(x_use + 10, y_start + 5);

	dc->SelectObject(pen_save);
	pen->DeleteObject();
	delete pen;
} // show_peak_edit()


void
ecg_view_mfc::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (_in_edit_mode && (point.x < EDIT_SPACE))
	{
//		mouse_press_edit_area(point.x, point.y);
		return;
	}

	int ch = mouse_press(point.x, point.y, (nFlags & MK_SHIFT ? true : false), false);
	CDC *dc = GetDC();
	plot_cursor(dc, ch);
	ReleaseDC(dc);

	if (!_in_edit_mode)
		_popup_menu_ecg.EnableMenuItem(IDM_TOGGLE_EDIT_MODE, (_ev_idx == -1 ? MF_GRAYED : MF_ENABLED));

	ts_view_mfc::OnLButtonDown(nFlags, point);
} // OnLButtonDown()


void
ecg_view_mfc::plot_cursor(CDC * dc, int ch/* = -1*/)
{
	static long pos_sel_save = -2;

	_cursor_ch = ch;

	if ((_cursor_pos < _left_space) || (_cursor_pos > (_screen_width - _right_space)))
		return;

	ts_view_mfc::plot_cursor(dc, ch);

	if ((_ev_idx != -1) && _prop_pos && _prop_class)
	{
		send_event();

		ra_prop_get_value(_prop_pos, _event_ids[_ev_idx], -1, _vh);
		long pos = ra_value_get_long(_vh);
		ra_prop_get_value(_prop_class, _event_ids[_ev_idx], -1, _vh);
		int cl = (int)ra_value_get_long(_vh);

		int x = (int) ((double) (pos - _curr_pos) * _max_xscale) + _left_space;
		HFONT font_save = (HFONT) dc->SelectObject(_font_bold);
		UINT ta_save = dc->SetTextAlign(TA_CENTER | TA_BASELINE);
		dc->SetBkMode(TRANSPARENT);
		char *c = raecg_get_class_string(cl);
		dc->TextOut(x, 13, c, strlen(c));
		dc->SetTextAlign(ta_save);
		dc->SelectObject(font_save);
	}
} // plot_cursor()


void
ecg_view_mfc::sort_general(prop_handle ph, double value)
{
	ecg_view_general::sort_general(ph, value);
	Invalidate();
}				// sort_general()

void
ecg_view_mfc::sort_ch_found(int ch)
{
	ecg_view_general::sort_ch_found(ch);
	Invalidate();
}				// sort_ch_found()


void
ecg_view_mfc::sort_template(int templ_num)
{
	ecg_view_general::sort_template(templ_num);
	Invalidate();
}				// sort_template()


void
ecg_view_mfc::sort_rri(int short_first)
{
	ecg_view_general::sort_rri(short_first ? true : false, 0);
	Invalidate();
}				// sort_rri()


void
ecg_view_mfc::sort_class(int classification)
{
	ecg_view_general::sort_class(classification);
	Invalidate();
}				// sort_class


void
ecg_view_mfc::_sort_short_rri()
{
	sort_rri(1);
}  // _sort_short_rri()


void
ecg_view_mfc::_sort_long_rri()
{
	sort_rri(0);
}  // _sort_long_rri()


void
ecg_view_mfc::_sort_normal()
{
	sort_class(ECG_CLASS_SINUS);
}  // _sort_normal()


void
ecg_view_mfc::_sort_vpc()
{
	sort_class(ECG_CLASS_VENT);
}  // _sort_vpc()


void
ecg_view_mfc::_sort_paced()
{
	sort_class(ECG_CLASS_PACED);
}  // _sort_paced()


void
ecg_view_mfc::_sort_artifact()
{
	sort_class(ECG_CLASS_ARTIFACT);
}  // _sort_artifact()


void
ecg_view_mfc::_sort_ch_use(UINT id)
{
	sort_ch_found(id - ECG_VIEW_CH_START);
}  // _sort_ch_use()


void
ecg_view_mfc::_sort_template(UINT id)
{
	sort_template(id - ECG_VIEW_TEMPLATE_START);
}  // _sort_template()


void
ecg_view_mfc::plot_edit_area(CDC *dc)
{
	if (_edit_beat.event_id < 0)
	{
		dc->TextOut(10, _screen_height/2, gettext("no beat selected"));
		return;
	}
	if (!_prop_morph_flags)
	{
		dc->TextOut(10, _screen_height/2, gettext("beat morphology data is not available"));
		return;
	}

	CPoint *pts = new CPoint[_edit_beat.num_data];
	for (int i = 0; i < _edit_beat.num_data; i++)
	{
		pts[i].x = (int)((double)i * _edit_beat.xscale) + 10;
		pts[i].y = (int)((_edit_beat.max - _edit_beat.data[i]) * _edit_beat.yscale) + EDIT_SPACE_VERT;
	}
	dc->Polyline(pts, _edit_beat.num_data);

	if (_show_lowpass_edit_signal)
	{
		for (int i = 0; i < _edit_beat.num_data; i++)
		{
			pts[i].x = (int)((double)i * _edit_beat.xscale) + 10;
			pts[i].y = (int)((_edit_beat.max - _edit_beat.data_filt[i]) * _edit_beat.yscale) + EDIT_SPACE_VERT;
		}

		CPen *pen = new CPen;
		pen->CreatePen(PS_SOLID, 1, RGB(255,0,0));
		CPen *pen_save = (CPen *)dc->SelectObject(pen);

		dc->Polyline(pts, _edit_beat.num_data);
		
		dc->SelectObject(pen_save);

		pen->DeleteObject();
		delete pen;
	}
	if (_show_highpass_edit_signal)
	{
		for (int i = 0; i < _edit_beat.num_data; i++)
		{
			pts[i].x = (int)((double)i * _edit_beat.xscale) + 10;
			pts[i].y = (int)((_edit_beat.max - _edit_beat.data_filt2[i]) * _edit_beat.yscale) + EDIT_SPACE_VERT;
		}

		CPen *pen = new CPen;
		pen->CreatePen(PS_SOLID, 1, RGB(0,0,255));
		CPen *pen_save = (CPen *)dc->SelectObject(pen);

		dc->Polyline(pts, _edit_beat.num_data);
		
		dc->SelectObject(pen_save);

		pen->DeleteObject();
		delete pen;
	}

	delete[] pts;

	if (ra_prop_get_value(_prop_morph_flags, _edit_beat.event_id, _ch[_curr_channel].ch_number, _vh) != 0)
		return;

	_edit_beat.morph_flags = ra_value_get_long(_vh);

	if (!IS_ARTIFACT(_edit_beat.annot) && !IS_UNKNOWN(_edit_beat.annot))
	{
		if ((_edit_beat.annot != -1) && IS_SINUS(_edit_beat.annot))
			show_p_wave_info(dc, _edit_beat.event_id, _edit_beat.ch, _edit_beat.pos, _edit_beat.morph_flags, true);
		show_qrs_info(dc, _edit_beat.event_id, _edit_beat.ch, _edit_beat.pos, _edit_beat.morph_flags, true);
		show_t_wave_info(dc, _edit_beat.event_id, _edit_beat.ch, _edit_beat.pos, _edit_beat.morph_flags, true);
	}

	print_edit_beat_info(dc);
} // plot_edit_area()


void
ecg_view_mfc::print_edit_beat_info(CDC *dc)
{
	int idx = _p_type_select.GetCurSel();
	switch (_edit_beat.p_type)
	{
	case -1:  // fall through
	case 0:
		if (idx != 0)
			_p_type_select.SetCurSel(0);
		break;
	case MORPH_TYPE_NEG:
		if (idx != MORPH_TYPE_NEG_IDX)
			_p_type_select.SetCurSel(MORPH_TYPE_NEG_IDX);
		break;
	case MORPH_TYPE_POS:
		if (idx != MORPH_TYPE_POS_IDX)
			_p_type_select.SetCurSel(MORPH_TYPE_POS_IDX);
		break;
	case MORPH_TYPE_BI_NEG_POS:
		if (idx != MORPH_TYPE_BI_NEG_POS_IDX)
			_p_type_select.SetCurSel(MORPH_TYPE_BI_NEG_POS_IDX);
		break;
	case MORPH_TYPE_BI_POS_NEG:
		if (idx != MORPH_TYPE_BI_POS_NEG_IDX)
		_p_type_select.SetCurSel(MORPH_TYPE_BI_POS_NEG_IDX);
			break;
	default:
		if (idx != 5)
			_p_type_select.SetCurSel(5);
		break;
	}

	idx = _qrs_type_select.GetCurSel();
	switch (_edit_beat.qrs_type)
	{
	case -1:  // fall through
	case 0:
		if (idx != 0)
			_qrs_type_select.SetCurSel(0);
		break;
	case MORPH_TYPE_S:
		if (idx != MORPH_TYPE_S_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_S_IDX);
		break;
	case MORPH_TYPE_R:
		if (idx != MORPH_TYPE_R_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_R_IDX);
		break;
	case MORPH_TYPE_QR:
		if (idx != MORPH_TYPE_QR_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_QR_IDX);
		break;
	case MORPH_TYPE_RS:
		if (idx != MORPH_TYPE_RS_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_RS_IDX);
		break;
	case MORPH_TYPE_QRS:
		if (idx != MORPH_TYPE_QRS_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_QRS_IDX);
		break;
	case MORPH_TYPE_RSR:
		if (idx != MORPH_TYPE_RSR_IDX)
			_qrs_type_select.SetCurSel(MORPH_TYPE_RSR_IDX);
		break;
	default:
		if (idx != 7)
			_qrs_type_select.SetCurSel(7);
		break;
	}

	idx = _t_type_select.GetCurSel();
	switch (_edit_beat.t_type)
	{
	case -1:  // fall through
	case 0:
		if (idx != 0)
			_t_type_select.SetCurSel(0);
		break;
	case MORPH_TYPE_NEG:
		if (idx != MORPH_TYPE_NEG_IDX)
			_t_type_select.SetCurSel(MORPH_TYPE_NEG_IDX);
		break;
	case MORPH_TYPE_POS:
		if (idx != MORPH_TYPE_POS_IDX)
			_t_type_select.SetCurSel(MORPH_TYPE_POS_IDX);
		break;
	case MORPH_TYPE_BI_NEG_POS:
		if (idx != MORPH_TYPE_BI_NEG_POS_IDX)
			_t_type_select.SetCurSel(MORPH_TYPE_BI_NEG_POS_IDX);
		break;
	case MORPH_TYPE_BI_POS_NEG:
		if (idx != MORPH_TYPE_BI_POS_NEG_IDX)
			_t_type_select.SetCurSel(MORPH_TYPE_BI_POS_NEG_IDX);
		break;
	default:
		if (idx != 5)
			_t_type_select.SetCurSel(5);
		break;
	}

	UINT ta_save = dc->SetTextAlign(TA_LEFT | TA_BASELINE);

	CString s = gettext("P-Wave");
	dc->TextOut(10, 25, s);
	_p_type_select.ShowWindow(SW_SHOW);

	s = gettext("QRS-Complex");
	dc->TextOut(10, 60, s);
	_qrs_type_select.ShowWindow(SW_SHOW);

	s = gettext("T-Wave");
	dc->TextOut(10, 95, s);
	_t_type_select.ShowWindow(SW_SHOW);

	dc->SetTextAlign(ta_save);

	_edit_apply.MoveWindow(10, _screen_height - 50, 100, 30, true);
	_edit_apply.ShowWindow(SW_SHOW);

	_edit_apply_all.MoveWindow(120, _screen_height - 50, 100, 30, true);
	_edit_apply_all.ShowWindow(SW_SHOW);

	_edit_close.MoveWindow(EDIT_SPACE - 110, _screen_height - 50, 100, 30, true);
	_edit_close.ShowWindow(SW_SHOW);
}  // print_edit_beat_info()


void
ecg_view_mfc::_p_type_changed()
{
	int idx = _p_type_select.GetCurSel();
	if (idx == LB_ERR)
		return;

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
	SetFocus();

	_edit_apply.EnableWindow();
	_edit_apply_all.EnableWindow();
} // slot_p_type_changed()


void
ecg_view_mfc::_qrs_type_changed()
{
	int idx = _qrs_type_select.GetCurSel();
	if (idx == LB_ERR)
		return;

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
	SetFocus();

	_edit_apply.EnableWindow();
	_edit_apply_all.EnableWindow();
} // slot_qrs_type_changed()


void
ecg_view_mfc::_t_type_changed()
{
	int idx = _t_type_select.GetCurSel();
	if (idx == LB_ERR)
		return;

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
	SetFocus();

	_edit_apply.EnableWindow();
	_edit_apply_all.EnableWindow();
} // slot_t_type_changed()


void
ecg_view_mfc::_edit_apply_pressed()
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
	SetFocus();

	_edit_apply.EnableWindow(false);
	_edit_apply_all.EnableWindow(false);
} // _edit_apply_pressed()


void
ecg_view_mfc::_edit_apply_all_pressed()
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
	SetFocus();

	_edit_apply.EnableWindow(false);
	_edit_apply_all.EnableWindow(false);
} // _edit_apply_all_pressed()


void
ecg_view_mfc::_edit_close_pressed()
{
	toggle_edit_mode();
	update();
}  // _edit_close_pressed()


void
ecg_view_mfc::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	int ch = mouse_press(point.x, point.y, (nFlags & MK_SHIFT ? true : false), true);

	ts_view_mfc::OnLButtonDblClk(nFlags, point);

	if (_ev_idx < 0)
		return;

	if (!_in_edit_mode)
		toggle_edit_mode();

	update();
}  // OnLButtonDblClk()


int
ecg_view_mfc::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (ts_view_mfc::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	init_edit();
	
	return 0;
}  // OnCreate()


void
ecg_view_mfc::OnSize(UINT nType, int cx, int cy)
{
	_screen_height = cy;
	if (_in_edit_mode)
		fill_edit_beat_data();

	ts_view_mfc::OnSize(nType, cx, cy);
}  // OnSize()


void
ecg_view_mfc::_toggle_edit_mode_menu()
{
	toggle_edit_mode();
	_popup_menu_ecg.CheckMenuItem(IDM_TOGGLE_EDIT_MODE, (_in_edit_mode ? MF_CHECKED : MF_UNCHECKED));
	update();
}  // _toggle_edit_mode_menu()
