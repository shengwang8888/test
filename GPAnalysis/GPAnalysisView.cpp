
// GPAnalysisView.cpp : implementation of the CGPAnalysisView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "GPAnalysis.h"
#endif

#include "GPAnalysisDoc.h"
#include "GPAnalysisView.h"

#include "MainFrm.h"
#include "GPMgr.h"
#include "GPStrategy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGPAnalysisView

IMPLEMENT_DYNCREATE(CGPAnalysisView, CView)

BEGIN_MESSAGE_MAP(CGPAnalysisView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CGPAnalysisView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_FILE_OPEN, &CGPAnalysisView::OnFileOpen)
	ON_MESSAGE(WM_USER_SHOW_GP, &CGPAnalysisView::OnAnalysisSingleGP)
ON_WM_KEYDOWN()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CGPAnalysisView construction/destruction

CGPAnalysisView::CGPAnalysisView()
{
	// TODO: add construction code here

}

CGPAnalysisView::~CGPAnalysisView()
{
}

BOOL CGPAnalysisView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CGPAnalysisView drawing

void CGPAnalysisView::OnDraw(CDC* pDC)
{
	CGPAnalysisDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CMemDC		memDC( *pDC, this );
	CDC *       pMDC = &memDC.GetDC();
	CRect		rcView;

	GetClientRect( &rcView );
	pMDC->FillSolidRect( &rcView, 0 );
	
	gpMgr.Draw_GP_UI(pMDC, rcView, m_ptMouse);

}


// CGPAnalysisView printing


void CGPAnalysisView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CGPAnalysisView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CGPAnalysisView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CGPAnalysisView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CGPAnalysisView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CGPAnalysisView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CGPAnalysisView diagnostics

#ifdef _DEBUG
void CGPAnalysisView::AssertValid() const
{
	CView::AssertValid();
}

void CGPAnalysisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGPAnalysisDoc* CGPAnalysisView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGPAnalysisDoc)));
	return (CGPAnalysisDoc*)m_pDocument;
}
#endif //_DEBUG


// CGPAnalysisView message handlers


void CGPAnalysisView::OnFileOpen()
{
	CFolderPickerDialog dlg;

	//if( dlg.DoModal() == IDOK )
	//{
	//	gIniSetting.strDataFolder = dlg.GetPathName();
	//	TRACE( "Set data folder: %s\n", gIniSetting.strDataFolder );
	//}

	CMainFrame *pWnd = (CMainFrame *)AfxGetMainWnd();
	//pWnd->m_wndClassView;
}

LRESULT CGPAnalysisView::OnAnalysisSingleGP(WPARAM wParam, LPARAM lParam)
{
	CGPAnalysisDoc *pDoc = GetDocument();

	CString *pGpFile = (CString *)wParam;
	//AfxMessageBox((CString)"Msg detected:\n" + *pGpFile);


	CGP *pGP = new CGP(pGpFile->GetString());

	GPStrategy_6010 StrategyAnalyer;
	StrategyAnalyer.do_strategy_analysis(0, 0, pGP);

	GPStrategyReport *pReport = StrategyAnalyer.get_strategy_report();
	int ret, precent;
	ret = pReport->GetTotalProfit();
	ret = pReport->GetLostTimes(&precent);
	ret = pReport->GetWinTimes(&precent);

	Invalidate(0);

	return 0;
}


void CGPAnalysisView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
		break;
	case VK_RIGHT:
		break;
	case VK_UP:
		gpMgr.UI_Zoom(ZOOMIN);
		break;
	case VK_DOWN:
		gpMgr.UI_Zoom(ZOOMOUT);
		break;
	}

	Invalidate(0);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CGPAnalysisView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ptMouse = point;
	Invalidate(0);

	CView::OnMouseMove(nFlags, point);
}
