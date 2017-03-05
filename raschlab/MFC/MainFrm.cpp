// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "raschlab_mfc.h"

#include "MainFrm.h"

#include "process_dlg.h"
process_dlg dlg;

#include <ra_ap_morphology.h>
#include <ra_fiducial_point.h>
#include <ra_template.h>
#include <ra_respiration.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_SIZE()
	ON_COMMAND(ID_DETECT_DETECT, OnDetectDetect)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_GENERAL_EVALUATIONINFOS, OnGeneralEvaluationinfos)
	ON_COMMAND(ID_DETECT_MAKECLASSIFICATIONS, OnDetectMakeclassifications)
	ON_COMMAND(ID_VIEW_PLOT, OnViewPlot)
	ON_COMMAND(ID_DETECT_MAKEBLOODPRESSURE, OnDetectMakebloodpressure)
	ON_WM_CLOSE()
	ON_COMMAND(ID_GENERAL_PLUGININFOS, show_plugin_infos)
	ON_COMMAND(ID_DETECT_PERFORMCALCULATIONS, OnDetectPerformcalculations)
	ON_COMMAND(ID_DETECT_DETECTCTG, detect_ctg)
	ON_COMMAND(ID_GENERAL_SAVEASCIIEVALVALUES, OnGeneralSaveasciievalvalues)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
	CRASCHlab_mfcApp *app = (CRASCHlab_mfcApp *)AfxGetApp();
	_ra = app->get_ra_handle();
	_settings = &(app->_settings);

	// create general process dialog
	dlg.Create(IDD_PROGRESS_DLG, this);

	m_bAutoMenuEnable = FALSE;
}  // constructor


CMainFrame::~CMainFrame()
{
}  // destructor


int
CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	// ZU ERLEDIGEN: Löschen Sie diese drei Zeilen, wenn Sie nicht wollen, dass die Symbolleiste
	//  andockbar ist.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	translate_menus();

	if (_settings->save_eval_ascii())
	{
		CMenu* mmenu = GetMenu();
		CMenu* submenu = mmenu->GetSubMenu(1);
		submenu->CheckMenuItem(ID_GENERAL_SAVEASCIIEVALVALUES, MF_CHECKED | MF_BYCOMMAND);
	}

	return 0;
}  // OnCreate()


void
CMainFrame::translate_menus()
{
	CMenu* mmenu = GetMenu();

	// file menu
	mmenu->ModifyMenu(0, MF_BYPOSITION, 0, gettext("&File"));
	CMenu* submenu = mmenu->GetSubMenu(0);
	submenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND, ID_FILE_OPEN,
						gettext("&Open...\tCtrl+O"));

	submenu->ModifyMenu(ID_FILE_CLOSE, MF_BYCOMMAND, ID_FILE_CLOSE,
						gettext("&Close\tCtrl+C"));

	submenu->ModifyMenu(ID_APP_EXIT, MF_BYCOMMAND, ID_APP_EXIT,
						gettext("E&xit"));

	// general menu
	mmenu->ModifyMenu(1, MF_BYPOSITION, 1, gettext("&General"));
	submenu = mmenu->GetSubMenu(1);
	submenu->ModifyMenu(ID_GENERAL_PLUGININFOS, MF_BYCOMMAND, ID_GENERAL_PLUGININFOS,
						gettext("&Plugin infos..."));

	submenu->ModifyMenu(ID_GENERAL_EVALUATIONINFOS, MF_BYCOMMAND, ID_GENERAL_EVALUATIONINFOS,
						gettext("&Evaluation infos..."));

	submenu->ModifyMenu(ID_GENERAL_SAVEASCIIEVALVALUES, MF_BYCOMMAND, ID_GENERAL_SAVEASCIIEVALVALUES,
						gettext("Save &ASCII eval-values"));

	// detect menu
	mmenu->ModifyMenu(2, MF_BYPOSITION, 2, gettext("&Detect"));
	submenu = mmenu->GetSubMenu(2);
	submenu->ModifyMenu(ID_DETECT_PERFORMCALCULATIONS, MF_BYCOMMAND, ID_DETECT_PERFORMCALCULATIONS,
						gettext("&Perform Calculations"));

	submenu->ModifyMenu(ID_DETECT_DETECT, MF_BYCOMMAND, ID_DETECT_DETECT,
						gettext("&Detect"));
	
	submenu->ModifyMenu(ID_DETECT_DETECTCTG, MF_BYCOMMAND, ID_DETECT_DETECTCTG,
						gettext("Detect C&TG"));
	
	submenu->ModifyMenu(ID_DETECT_MAKECLASSIFICATIONS, MF_BYCOMMAND, ID_DETECT_MAKECLASSIFICATIONS,
						gettext("&Classify ECG"));
	
	submenu->ModifyMenu(ID_DETECT_MAKEBLOODPRESSURE, MF_BYCOMMAND, ID_DETECT_MAKEBLOODPRESSURE,
						gettext("&RR-Morphology"));

	// view menu
	mmenu->ModifyMenu(3, MF_BYPOSITION, 3, gettext("&View"));
	submenu = mmenu->GetSubMenu(3);
	submenu->ModifyMenu(ID_VIEW_TOOLBAR, MF_BYCOMMAND, ID_VIEW_TOOLBAR,
						gettext("Tool&bar"));

	submenu->ModifyMenu(ID_VIEW_STATUS_BAR, MF_BYCOMMAND, ID_VIEW_STATUS_BAR,
						gettext("&Status Bar"));

	submenu->ModifyMenu(ID_VIEW_PLOT, MF_BYCOMMAND, ID_VIEW_PLOT,
						gettext("&Plot..."));

	// help menu
	mmenu->ModifyMenu(4, MF_BYPOSITION, 4, gettext("&Help"));
	submenu = mmenu->GetSubMenu(4);
	submenu->ModifyMenu(ID_APP_ABOUT, MF_BYCOMMAND, ID_APP_ABOUT,
						gettext("&About RASCHlab-MFC..."));
}  // translate_menus()



BOOL
CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	_settings->get_last_path(_last_path, MAX_PATH);

	return TRUE;
}  // PreCreateWindow()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Diagnose

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Nachrichten-Handler


void
CMainFrame::OnFileOpen() 
{
	CString sig_fn = select_signal();
	if (sig_fn.IsEmpty())
		return;

	open_sig(sig_fn);
}  // OnFileOpen()


void
CMainFrame::open_sig(const char *fn)
{
	CWaitCursor wait;

	signal_ra_mfc *sig = new signal_ra_mfc(_ra);
	if (!sig->open(fn))
	{
		delete sig;
		return;
	}
	_sig.insert(_sig.end(), sig);

	AfxGetApp()->AddToRecentFileList(fn);

	make_detect_menu(sig);

	open_sig_view(sig);
	open_template_view(sig);
	open_plot_view(sig);

	pos_views();
}  // open_sig()


void
CMainFrame::make_detect_menu(signal_ra_mfc *sig)
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(2);
	submenu->EnableMenuItem(ID_DETECT_DETECT, MF_GRAYED);
	submenu->EnableMenuItem(ID_DETECT_DETECTCTG, MF_GRAYED);
	submenu->EnableMenuItem(ID_DETECT_MAKECLASSIFICATIONS, MF_GRAYED);
	submenu->EnableMenuItem(ID_DETECT_MAKEBLOODPRESSURE, MF_GRAYED);

	if (sig == NULL)
		return;

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
				submenu->EnableMenuItem(ID_DETECT_DETECT, MF_ENABLED);
				submenu->EnableMenuItem(ID_DETECT_MAKECLASSIFICATIONS, MF_ENABLED);
				ecg = true;
			}
			break;
		case RA_CH_TYPE_RR:
			if (!rr)
			{
				submenu->EnableMenuItem(ID_DETECT_MAKEBLOODPRESSURE, MF_ENABLED);
				rr = true;
			}
			break;
		case RA_CH_TYPE_CTG_UC:
			if (!ctg)
			{
				submenu->EnableMenuItem(ID_DETECT_DETECTCTG, MF_ENABLED);
				ctg = true;
			}
			break;
		}
	}
	ra_value_free(vh);
} // make_detect_menu()


bool
CMainFrame::open_sig_view(signal_ra_mfc *sig)
{
	plugin_handle p = ra_plugin_get_by_name(_ra, "rasch-view", 1);
	if (!p)
		return false;

	view_handle vih = ra_view_get(sig->get_meas_handle(), p, this);
	if (ra_view_create(vih) != 0)
		return false;

	CWnd *wnd = (CWnd *)ra_view_get_window(vih, 0);
	sig->add_view(wnd, vih, SIGNAL_VIEW);

	char title[200];
	sig->get_title(title, 200);
	strcat(title, " - signals");
	wnd->SetWindowText(title);

	wnd->ShowWindow(SW_SHOW);

	return true;
}  // open_sig_view()


bool
CMainFrame::open_template_view(signal_ra_mfc *sig)
{
	// check if evaluation is done
	eval_handle eh = ra_eval_get_default(sig->get_meas_handle());
	if (!eh)
		return false;

	// check if ev-summary-view plugin is available
	plugin_handle p = ra_plugin_get_by_name(_ra, "ev-summary-view", 1);
	if (!p)
		return false;

	// check if templates in the evaluation
	CWnd *templ_view = NULL;
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
			if (clh && (templ_view == NULL) && (ra_prop_get(clh, "uc-template") != NULL))
				templ_view = show_uc_templates(sig);
	}
	ra_value_free(vh);
	
	if (templ_view)
		templ_view->ShowWindow(SW_SHOW);

	return (templ_view ? true : false);	
}  // open_template_view()


CWnd *
CMainFrame::show_qrs_templates(signal_ra_mfc *sig, sum_handle sh)
{	
	plugin_handle p = ra_plugin_get_by_name(_ra, "ev-summary-view", 1);
	if (!p)
		return NULL;
	
	CChildFrame *view = new CChildFrame();
	view->Create(NULL, NULL);

	view_handle vih = ra_view_get(sig->get_meas_handle(), p, view);
	if (vih == NULL)
		return NULL;
	
	value_handle vh = ra_value_malloc();
	ra_value_set_voidp(vh, sh);
	ra_lib_set_option(vih, "sh", vh);
	ra_value_free(vh);

	if (ra_view_create(vih) != 0)
		return NULL;
	
	view->set_child((CWnd *)ra_view_get_window(vih, 0));
	sig->add_view(view, vih, TEMPLATE_VIEW);

	char title[200];
	sig->get_title(title, 200);
	strcat(title, " - qrs-templates");
	view->SetWindowText(title);

	view->ShowWindow(SW_SHOW);

	return view;
}  // show_qrs_templates()


CWnd *
CMainFrame::show_uc_templates(signal_ra_mfc *sig)
{	
	/*eval_handle eh = ra_eval_get_def(sig->get_meas_handle());

	value_handle vh = ra_value_malloc();
	prop_handle proph = ra_prop_get_by_name(eh, "uc-template");
	ra_info_get(proph, RA_INFO_TEMPL_NUM_L, vh);
	if ((proph == NULL) || (ra_value_get_long(vh) <= 0))
	{
		ra_value_free(vh);
		return NULL;
	}
	plugin_handle p = ra_plugin_get_by_name(_ra, "ev-summary-view", 1);
	if (!p)
		return NULL;
	

	CChildFrame *view = new CChildFrame();
	view->Create(NULL, NULL);
	view->set_meas_handle(sig->get_meas_handle());

	struct view_info vi;
	vi.ra = _ra;
	vi.mh = sig->get_meas_handle();
	vi.plugin = p;
	vi.parent = view;
	vi.name[0] = '\0';
	vi.signal = NULL;
	vi.eh = sig->get_eval_handle();
	strcpy(vi.pos_name, "uc-max-pos");
	strcpy(vi.templ_name, "uc-template");
	if (ra_view_get(&vi) != 0)
	{
		delete view;
		return false;
	}

	view->set_child((CWnd *)(vi.views[0]));
	sig->add_view(view, TEMPLATE_VIEW);

	char title[200];
	sig->get_title(title, 200);
	strcat(title, " - uc-templates");
	view->SetWindowText(title);

	view->ShowWindow(SW_SHOW);

	ra_value_free(vh);
	
	return view;
	*/
	return NULL;
}  // show_uc_templates()


bool
CMainFrame::open_plot_view(signal_ra_mfc *sig, struct ra_plot_options *opt/*=NULL*/)
{
	// check if evaluation is done
	eval_handle eh = ra_eval_get_default(sig->get_meas_handle());
	if (!eh)
		return false;

	// check if ra-plot plugin is available
	plugin_handle p = ra_plugin_get_by_name(_ra, "plot-view", 1);
	if (!p)
		return false;

	if (!opt)
	{
		opt = new struct ra_plot_options;
		bool ok = fill_plot_infos(sig, opt);
		if (!ok)
		{
			delete opt;
			return false;
		}
	}

	CChildFrame *view = new CChildFrame();
	view->Create(NULL, NULL);
	view->set_meas_handle(sig->get_meas_handle());

	view_handle vih = ra_view_get(sig->get_meas_handle(), p, view);
	struct view_info *vi = (struct view_info *)vih;
    vi->options = opt;
	if (ra_view_create(vih) != 0)
	{
		delete view;
		ra_view_free(vih);
		return false;
	}

	view->set_child((CWnd *)ra_view_get_window(vih, 0));
	sig->add_view(view, vih, PLOT_VIEW);

	char title[200];
	sig->get_title(title, 200);
	strcat(title, " - plot");
	view->SetWindowText(title);

	view->ShowWindow(SW_SHOW);

	return true;
}  // open_plot_view()


void
CMainFrame::close_sig(signal_ra_mfc *sig)
{
	sig->close();
	_sig.remove(sig);
	delete sig;

	make_detect_menu(NULL);
}  // close_sig()


CString
CMainFrame::select_signal()
{
	CString sig = "";

	CRASCHlab_mfcApp *app = (CRASCHlab_mfcApp *)AfxGetApp();
	ra_handle ra = app->get_ra_handle();

	plugin_handle plugin = ra_plugin_get_by_name(ra, "sig-sel-dlg", 1);
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

	strcpy(_last_path, sig);
	char *p = strrchr(_last_path, '\\');
	if (p)
		*p = '\0';

	return sig;	
}  // select_signal()


void
CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIFrameWnd::OnSize(nType, cx, cy);

	pos_views();
}  // OnSize()


void
CMainFrame::pos_views()
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;

	int n = sig->num_views();	
	
	if (n <= 1)
	{
		CWnd *v = (CWnd *)sig->get_view(NULL);
		v->ShowWindow(SW_SHOWMAXIMIZED);
		return;
	}

	bool no_template_view = true;
	CWnd *v = NULL;
	int i;
	for (i = 0; i < n; i++)
	{
		v = (CWnd *)sig->get_view(v);
		if (!v)
			break;
		
		if (sig->get_view_type(v) == TEMPLATE_VIEW)
		{
			no_template_view = false;
			break;
		}
	}

	CMDIChildWnd *child = MDIGetActive();
	RECT r;
	child->GetParent()->GetClientRect(&r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	int pv_start, pv_height;
	if (sig->num_plot_views() > 0)
	{
		pv_start = (int)((double)(height) * (sig->num_plot_views() >= 2 ? 0.33 : 0.66));
		pv_height = (height - pv_start) / sig->num_plot_views();
	}

	v = NULL;
	int num_plot_view_shown = 0;
	for (i = 0; i < n; i++)
	{
		v = (CWnd *)sig->get_view(v);
		if (!v)
			break;
		
		int type = sig->get_view_type(v);
		int x, y, w, h;
		bool dont_move = false;
		switch (type)
		{
		case SIGNAL_VIEW:
			x = y = 0;
			w = width;
			if (sig->num_plot_views() >= 2)
				h = (int)((double)height * 0.33);
			else
				h = (int)((double)height * 0.66);
			break;
		case TEMPLATE_VIEW:
			x = 0;
			y = (int)((double)height * 0.66);
			if (sig->num_plot_views() == 0)
				w = width;
			else
				w = (int)((double)width * 0.33);
			h = height - y;
			break;
		case PLOT_VIEW:
			if (no_template_view)
				x = 0;
			else
				x = (int)((double)width * 0.33);
			w = width - x;
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
		
		v->MoveWindow(x, y, w, h);
	}
	sig->update_all_views(NULL);
}  // pos_views()


signal_ra_mfc *
CMainFrame::get_curr_sig()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CChildFrame *v = (CChildFrame *) pFrame->GetActiveFrame();

	list<signal_ra_mfc *>::iterator it;
	for (it = _sig.begin(); it != _sig.end(); it++)
	{
		if ((*it)->find_view(v) != -1)
			return *it;
	}

	return NULL;
}  // get_curr_sig()


void
process_msgs()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}  // process_msgs()


// FIXME: will be generated dynamically, later
void
detect_callback(const char *text, int percent)
{
	if (text)
		dlg.set_text(text);
	dlg.set_progress(percent);
	
	process_msgs();
}  // detect_callback


void CMainFrame::OnDetectDetect() 
{
	signal_ra_mfc *sig = get_curr_sig();
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

	dlg.ShowWindow(SW_SHOW);	
	process_msgs();

	/* prepare eval to store the detection results */
	eval_handle eh_save = ra_eval_get_default(mh);
	eval_handle eh = ra_eval_add(mh, "RASCHlab", "evaluation created within RASCHlab", 0);
	if (!eh)
	{
		dlg.ShowWindow(SW_HIDE);
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
		dlg.ShowWindow(SW_HIDE);
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
 	ra_eval_save(mh, NULL, _settings->save_eval_ascii());
		
	dlg.ShowWindow(SW_HIDE);
	delete[] ch_list.ch;

	char fn[MAX_PATH_RA];
	sig->file_name(fn, MAX_PATH_RA);

	close_sig(sig);

	open_sig(fn);
}  // OnDetectDetect()


void
CMainFrame::OnFileSave() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;

	sig->save(_settings->save_eval_ascii());
}  // OnFileSave()


void CMainFrame::OnFileClose() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;

	close_sig(sig);
}

void CMainFrame::OnGeneralEvaluationinfos() 
{
	signal_ra_mfc *sig = get_curr_sig();
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
	ra_gui_call(&pi, pi.plugin);
}  // OnGeneralEvaluationinfos()


void
CMainFrame::OnDetectMakeclassifications() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;
	char fn[MAX_PATH];
	sig->file_name(fn, MAX_PATH);

	process_signal("ecg");

	close_sig(sig);
	open_sig(fn);
}  // OnDetectMakeclassifications() 


void
CMainFrame::process_signal(char *plugin_name)
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	eval_handle eh = ra_eval_get_default(mh);
    	if (!eh)
        	return;

	dlg.ShowWindow(SW_SHOW);
	process_msgs();

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, plugin_name, 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, detect_callback);

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

	dlg.ShowWindow(SW_HIDE);
}  // process_signal()


bool
CMainFrame::fill_plot_infos(signal_ra_mfc *sig, struct ra_plot_options *opt)
{
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
CMainFrame::add_axis(struct ra_plot_options *opt)
{
	opt->num_axis++;
	struct ra_plot_axis *t = new struct ra_plot_axis[opt->num_axis];
	memset(t, 0, sizeof(struct ra_plot_axis) * opt->num_axis);
	if (opt->axis)
	{
		memcpy(t, opt->axis, sizeof(struct ra_plot_axis) * (opt->num_axis-1));
		delete[] opt->axis;
	}
	opt->axis = t;

	return &(opt->axis[opt->num_axis-1]);
}  // add_axis()


void
CMainFrame::add_pos_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "qrs-pos");
	curr->event_property = prop;
	strcpy(curr->unit, "min");
	curr->use_min = curr->use_max = 0;
	curr->is_time = 1;
	curr->time_format = RA_PLOT_TIME_MIN;
	curr->ch = -1;
}  // add_pos_axis()


void
CMainFrame::add_rri_axis(struct ra_plot_options *opt, prop_handle prop)
{
	struct ra_plot_axis *curr = add_axis(opt);

	strcpy(curr->prop_name, "rri");
	curr->event_property = prop;
	strcpy(curr->unit, "ms");
	curr->use_min = curr->use_max = 1;
	curr->min = 300;
	curr->max = 2000;
	curr->ch = -1;
}  // add_rr_axis()


void
CMainFrame::add_syst_axis(struct ra_plot_options *opt, prop_handle prop)
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
CMainFrame::add_dias_axis(struct ra_plot_options *opt, prop_handle prop)
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
CMainFrame::add_resp_axis(struct ra_plot_options *opt, prop_handle prop)
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
CMainFrame::add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, char *name,
	long r, long g, long b)
{
	opt->num_plot_pairs++;
	struct ra_plot_pair *t = new struct ra_plot_pair[opt->num_plot_pairs];
	memset(t, 0, sizeof(struct ra_plot_pair) * opt->num_plot_pairs);
	if (opt->pair)
	{
		memcpy(t, opt->pair, sizeof(struct ra_plot_pair) * (opt->num_plot_pairs-1));
		delete[] opt->pair;
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
CMainFrame::get_channel(class_handle clh, long ch_type, long **ch, long *num_ch)
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
CMainFrame::OnViewPlot() 
{
	return;

/*	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;

	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;
	
	struct view_info vi;
	memset(&vi, 0, sizeof(struct view_info));
	vi.parent = this;
	vi.mh = mh;
	vi.rh = ra_rec_get_first(mh, 0);
	vi.eh = ra_eval_get_default(mh);
        if (!vi.eh)
        	return;
	
	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "ra-plot", 1)) != NULL)
	{
		view_handle vih = ra_view_get(mh, pl, this);

		int ret = ra_gui_call(&vi, vi.plugin);
		if (ret != 0)
			return;

		struct ra_plot_options *opt = (struct ra_plot_options *)vi.options;
		open_plot_view(sig, opt);
	}

	pos_views();*/
}  // OnViewPlot()


void
CMainFrame::OnDetectMakebloodpressure() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;
	char fn[MAX_PATH];
	sig->file_name(fn, MAX_PATH);

	process_signal("ap-morphology");

	close_sig(sig);
	open_sig(fn);
}  // OnDetectMakebloodpressure()



void CMainFrame::OnClose() 
{
	RECT rect;
	GetWindowRect(&rect);

	if (IsZoomed())
		_settings->set_maximized(true);
	else
		_settings->set_maximized(false);

	_settings->set_pos_x(rect.left);
	_settings->set_pos_y(rect.top);
	_settings->set_size_w(rect.right - rect.left);
	_settings->set_size_h(rect.bottom - rect.top);
	_settings->set_last_path(_last_path);

	_settings->save();

	CMDIFrameWnd::OnClose();
}


void CMainFrame::show_plugin_infos() 
{
	plugin_handle p = ra_plugin_get_by_name(_ra, "plugin-info-dlg", 1);
	if (!p)
		return;
			
	struct proc_info pi;
	memset(&pi, 0, sizeof(struct proc_info));
	pi.plugin = p;
	ra_gui_call(&pi, pi.plugin);
}


void
CMainFrame::OnDetectPerformcalculations() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;

	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;
	eval_handle eh = ra_eval_get_default(mh);

	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "simple-calc-dlg", 1)) == NULL)
		return;

	struct proc_info *pi = (struct proc_info *)ra_proc_get(mh, pl, NULL);
	ra_gui_call(pi, pl);
	ra_proc_free(pi);
}  // OnDetectPerformcalculations()


void
CMainFrame::detect_ctg() 
{
	signal_ra_mfc *sig = get_curr_sig();
	if (!sig)
		return;
		
	meas_handle mh = sig->get_meas_handle();
	if (!mh)
		return;

	struct ra_ch_list ch_list;
	memset(&ch_list, 0, sizeof(struct ra_ch_list));

	return;
/*
	plugin_handle pl;
	if ((pl = ra_plugin_get_by_name(_ra, "ch-select-dlg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(pl);
		pi->plugin = pl;
		pi->mh = mh;
		pi->rh = ra_rec_get_first(mh, 0);
		pi->eh = ra_eval_get_def(mh);
		pi->callback = detect_callback;

		int ret = ra_gui_call(pi, pi->plugin);
		if (ret != 0)
		{
			ra_proc_free(pi);
			return;
		}

		value_handle vh = ra_value_malloc();
		ra_value_set_number(vh, 0);
		ra_proc_get_result(pi, vh);
		ch_list.num_ch = ra_value_get_num_elem(vh);
		ch_list.ch = new long[ch_list.num_ch];
		memcpy(ch_list.ch, ra_value_get_long_array(vh), sizeof(long) * ch_list.num_ch);
		ra_value_free(vh);

		ra_proc_free(pi);
	}
		
	dlg.ShowWindow(SW_SHOW);	
	process_msgs();
		
	if ((pl = ra_plugin_get_by_name(_ra, "detect-ctg", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(pl);
		pi->plugin = pl;
		pi->mh = mh;
		pi->rh = ra_rec_get_first(mh, 0);
		pi->eh = ra_eval_get_def(mh);
		pi->save_in_eval = 1;

		value_handle vh = ra_value_malloc();
		ra_value_set_long_array(vh, ch_list.ch, ch_list.num_ch);
		ra_lib_set_option(pi, "ch", vh);
		ra_value_free(vh);

		pi->callback = detect_callback;

		ra_proc_do(pi);
		ra_proc_free(pi);
	}

	eval_handle eh = ra_eval_get_def(mh);
	if (!eh)
	{
		dlg.ShowWindow(SW_HIDE);
       	return;
	}

	if ((pl = ra_plugin_get_by_name(_ra, "template", 1)) != NULL)
	{
		struct proc_info *pi = (struct proc_info *)ra_proc_get(pl);
		pi->plugin = pl;
		pi->mh = mh;
		pi->rh = ra_rec_get_first(mh, 0);
		pi->eh = eh;
		pi->save_in_eval = 1;
		pi->callback = detect_callback;

		value_handle vh = ra_value_malloc();
		ra_value_set_long(vh, 1);
		ra_lib_set_option(pi, "use_eval", vh);
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

	ra_eval_savexml(mh, NULL, _settings->save_eval_ascii());
		
	dlg.ShowWindow(SW_HIDE);	

	char fn[MAX_PATH];
	sig->file_name(fn, MAX_PATH);

	close_sig(sig);
	open_sig(fn);
	*/
}  // detect_ctg()


void
CMainFrame::OnGeneralSaveasciievalvalues() 
{
	CMenu* mmenu = GetMenu();
	CMenu* submenu = mmenu->GetSubMenu(1);

	UINT state = submenu->GetMenuState(ID_GENERAL_SAVEASCIIEVALVALUES, MF_BYCOMMAND);
	ASSERT(state != 0xFFFFFFFF);

	if (state & MF_CHECKED)
		submenu->CheckMenuItem(ID_GENERAL_SAVEASCIIEVALVALUES, MF_UNCHECKED | MF_BYCOMMAND);
	else
		submenu->CheckMenuItem(ID_GENERAL_SAVEASCIIEVALVALUES, MF_CHECKED | MF_BYCOMMAND);

	bool t = _settings->save_eval_ascii();
	_settings->set_save_eval_ascii(t ? false : true);
}  // OnGeneralSaveasciievalvalues()
