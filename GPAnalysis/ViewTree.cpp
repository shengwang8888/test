
#include "stdafx.h"
#include "ViewTree.h"

#include "MainFrm.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CViewTree::OnDblClkTree)
	ON_NOTIFY_REFLECT(NM_CLICK, &CViewTree::OnClkTree)
	ON_COMMAND(ID_EXPLORER_EXPORT, &CViewTree::OnExplorerExport)
	ON_COMMAND(ID_EXPLORER_EXPORTALL, &CViewTree::OnExplorerExportall)
	ON_COMMAND(ID_EXPLORER_ANALYSIS, &CViewTree::OnExplorerAnalysis)
	ON_COMMAND(ID_EXPLORER_ANALYSISALL, &CViewTree::OnExplorerAnalysisall)
	ON_WM_KEYUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CViewTree::GetSelectedFullName(CString &strFullName, int &type)
{
	HTREEITEM hRoot = GetRootItem();
	HTREEITEM hTreeItem = GetSelectedItem();
	HTREEITEM hParentItem = hTreeItem;

	//以GetItemText()函数为例： 

	strFullName = GetItemText(hTreeItem);

	while (hParentItem != hRoot)
	{
		hParentItem = GetParentItem(hParentItem);
		if (hParentItem == hRoot)
			strFullName = gSetting.m_DataSourcePath + "\\" + strFullName;
		else
			strFullName = GetItemText(hParentItem) + "\\" + strFullName;
	}

	int imgId, imgSelId;
	GetItemImage(hTreeItem, imgId, imgSelId);

	type = imgId;
}


void CViewTree::OnDblClkTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString   strFullName;
	int       imgType;
	//以GetItemText()函数为例：

	GetSelectedFullName(strFullName, imgType);

	if (imgType != 0)
	{
		//AfxMessageBox("You have selected " + strFullName);
		CMainFrame* pMainWnd = (CMainFrame *)(AfxGetApp()->GetMainWnd());
		::SendMessage(pMainWnd->GetActiveView()->m_hWnd, WM_USER_SHOW_GP, (WPARAM)&strFullName, 0);
	}

	*pResult = 0;
}

void CViewTree::OnClkTree(NMHDR* pNMHDR, LRESULT* pResult)
{

	HTREEITEM hTreeItem = GetSelectedItem();

	CString S1 = GetItemText(hTreeItem);
	//AfxMessageBox("You have selected " + S1);

	*pResult = 0;
}


void CViewTree::OnExplorerExport()
{
	// TODO: Add your command handler code here
}


void CViewTree::OnExplorerExportall()
{
	// TODO: Add your command handler code here
}


void CViewTree::OnExplorerAnalysis()
{
	// TODO: Add your command handler code here
}


void CViewTree::OnExplorerAnalysisall()
{
	// TODO: Add your command handler code here
}


void CViewTree::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString   strFullName;
	int       imgType;

	if (nChar == VK_RETURN)
	{
		GetSelectedFullName(strFullName, imgType);

		if (imgType != 0)
		{
			CMainFrame* pMainWnd = (CMainFrame *)(AfxGetApp()->GetMainWnd());
			::SendMessage(pMainWnd->GetActiveView()->m_hWnd, WM_USER_SHOW_GP, (WPARAM)&strFullName, 0);
		}
	}

	CTreeCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}
