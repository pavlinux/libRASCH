// ChildFrm.cpp : Implementierung der Klasse CChildFrame
//

#include "stdafx.h"
#include "raschlab_mfc.h"

#include "ChildFrm.h"
#include "MainFrm.h"
#include "signal_ra_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame Konstruktion/Destruktion

CChildFrame::CChildFrame()
{
	_child = NULL;
	_mh = NULL;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// ZU ERLEDIGEN: Ändern Sie hier die Fensterklasse oder das Erscheinungsbild, indem Sie
	//  CREATESTRUCT cs modifizieren.

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame Diagnose

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame Nachrichten-Handler

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	if (_child != NULL)
		_child->MoveWindow(0, 0, cx, cy);
}

void CChildFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (_child == NULL)	
		CMDIChildWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	else
	{
		WPARAM w = nChar;
		LPARAM l = nRepCnt | (nFlags >> 16);
		_child->SendMessage(WM_KEYDOWN, w, l);
	}
}

void
CChildFrame::OnDestroy() 
{
	static int in_destroy = 0;

	if (in_destroy)
		return;  // already here

	in_destroy = 1;

	CMainFrame *frm = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	signal_ra_mfc *sig = frm->get_curr_sig();
	if (sig)
		sig->remove_view(this, false, this->_child);

	CMDIChildWnd::OnDestroy();
}  // OnDestroy()

