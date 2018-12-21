
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
	ON_MESSAGE(WM_USER_ANALYSIS_SINGLE, &CGPAnalysisView::OnAnalysisSingleGP)
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
	CRect		rcView;
	int			i;

	GetClientRect( &rcView );
	memDC.GetDC().FillSolidRect( &rcView, 0 );
	
	for( i=0; i<rcView.Width()*rcView.Height()/4; i++ )
		memDC.GetDC().SetPixel( rand()%rcView.Width(), rand()%rcView.Height(), RGB(255,0,0) );

	

	// TODO: add draw code for native data here
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
	CString *pGpFile = (CString *)wParam;
	//AfxMessageBox((CString)"Msg detected:\n" + *pGpFile);

	CGPAnalysisDoc *pDoc = GetDocument();
	CGP *pGP = new CGP(pGpFile->GetString());

	return 0;
}

