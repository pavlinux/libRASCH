// MainFrm.h : Schnittstelle der Klasse CMainFrame
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__00622A39_2ABF_40CA_B6D4_CB8BC6D80587__INCLUDED_)
#define AFX_MAINFRM_H__00622A39_2ABF_40CA_B6D4_CB8BC6D80587__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChildFrm.h"
#include "signal_ra_mfc.h"

#include <ra_plot_structs.h>

#include <list>
#include <algorithm>

using namespace std;

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attribute
public:

// Operationen
public:

	void open_sig(const char *fn);
	void close_sig(signal_ra_mfc *sig);

	signal_ra_mfc *get_curr_sig();


// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementierung
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // Eingebundene Elemente der Steuerleiste
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

	ra_handle _ra;
	settings *_settings;
	char _last_path[MAX_PATH];
	list<signal_ra_mfc *> _sig;

	void translate_menus();

	void process_signal(char *plugin_name);

	void make_detect_menu(signal_ra_mfc *sig);

	CString select_signal();
	bool open_sig_view(signal_ra_mfc *sig);
	
	bool open_template_view(signal_ra_mfc *sig);
	CWnd *show_qrs_templates(signal_ra_mfc *sig, sum_handle sh);
	CWnd *show_uc_templates(signal_ra_mfc *sig);

	bool open_plot_view(signal_ra_mfc *sig, struct ra_plot_options *opt=NULL);
	void pos_views();

	bool fill_plot_infos(signal_ra_mfc *sig, struct ra_plot_options *opt);
	struct ra_plot_axis *add_axis(struct ra_plot_options *opt);
	void add_pos_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_rri_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_syst_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_dias_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_resp_axis(struct ra_plot_options *opt, prop_handle prop);
	void add_pair(struct ra_plot_options *opt, int x_idx, int y_idx, char *name,
		long r, long g, long b);
	int get_channel(class_handle clh, long ch_type, long **ch, long *num_ch);

// Generierte Message-Map-Funktionen
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpen();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDetectDetect();
	afx_msg void OnFileSave();
	afx_msg void OnFileClose();
	afx_msg void OnGeneralEvaluationinfos();
	afx_msg void OnDetectMakeclassifications();
	afx_msg void OnViewPlot();
	afx_msg void OnDetectMakebloodpressure();
	afx_msg void OnClose();
	afx_msg void show_plugin_infos();
	afx_msg void OnDetectPerformcalculations();
	afx_msg void detect_ctg();
	afx_msg void OnGeneralSaveasciievalvalues();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_MAINFRM_H__00622A39_2ABF_40CA_B6D4_CB8BC6D80587__INCLUDED_)
