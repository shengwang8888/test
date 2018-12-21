
#include "stdafx.h"
#include "ViewTree.h"

#include "MainFrm.h"

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

void CViewTree::OnDblClkTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	HTREEITEM hRoot = GetRootItem();
	HTREEITEM hTreeItem = GetSelectedItem();
	HTREEITEM hParentItem = hTreeItem;
	CString   strFullName;
	//ÒÔGetItemText()º¯ÊýÎªÀý£º 

	strFullName = GetItemText(hTreeItem);

	while (hParentItem != hRoot)
	{
		hParentItem = GetParentItem(hParentItem);
		if(hParentItem == hRoot )
			strFullName = gSetting.m_DataSourcePath + "\\" + strFullName;
		else
			strFullName = GetItemText(hParentItem) + "\\" + strFullName;
	}

	int imgId, imgSelId;
	GetItemImage(hTreeItem, imgId, imgSelId);

	if (imgId != 0)
	{
		//AfxMessageBox("You have selected " + strFullName);
		CMainFrame* pMainWnd = (CMainFrame *)(AfxGetApp()->GetMainWnd());
		::SendMessage(pMainWnd->GetActiveView()->m_hWnd, WM_USER_ANALYSIS_SINGLE, (WPARAM)&strFullName, 0);
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
