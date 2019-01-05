
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree window

class CViewTree : public CTreeCtrl
{
// Construction
public:
	CViewTree();

// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	void OnDblClkTree(NMHDR* pNMHDR, LRESULT* pResult);
	void OnClkTree(NMHDR* pNMHDR, LRESULT* pResult);

private:
	void GetSelectedFullName(CString &strFullName, int &type);

// Implementation
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnExplorerExport();
	afx_msg void OnExplorerExportall();
	afx_msg void OnExplorerAnalysis();
	afx_msg void OnExplorerAnalysisall();
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};
