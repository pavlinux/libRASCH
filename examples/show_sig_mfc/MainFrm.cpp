// MainFrm.cpp : Implementierung der Klasse CMainFrame
//

#include "stdafx.h"
#include "show_sig_mfc.h"

#include "MainFrm.h"

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame Konstruktion/Zerstörung

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// EXAMPLE-CODE (start)
	CShow_sig_mfcApp *app = (CShow_sig_mfcApp *)AfxGetApp();

	// get view-plugin 'rasch-view'
	plugin_handle pl = ra_plugin_get_by_name(app->_ra, "rasch-view", 0);
	if (!pl)
		return -1;

	struct view_info vi;
	memset(&vi, 0, sizeof(struct view_info));
	vi.m = app->_ra;
	vi.mh = app->_meas;
	vi.rh = ra_rec_get_first(vi.mh, 0);
	vi.plugin = pl;
	vi.parent = this;
	if (ra_view_get(&vi) != 0)
		return -1;

	CWnd *wnd = (CWnd *)(vi.views[0]);
	wnd->ShowWindow(SW_SHOW);
	// EXAMPLE-CODE (end)

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

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

