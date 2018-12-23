// DrawClass.h : main header file for the DRAWCLASS DLL
//

#if !defined(AFX_DRAWLIB_H__7DA53447_4DF1_4EB0_828D_42A6D4109F81__INCLUDED_)
#define AFX_DRAWLIB_H__7DA53447_4DF1_4EB0_828D_42A6D4109F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

//#include "resource.h"		// main symbols

#define _DLL_EXPORT_

#ifdef _DLL_EXPORT_
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

class DLL_API DrawLib
{
 public:

	/***************************************************************************************************/
	/*	描述：畫漸變色過渡處理的背景色
	/*  mode: 0=只畫邊框. 1=從上至下漸變. 2=從左至右. 3=左上到右下.4=從中間向兩邊(距形)
	/*  crStart:   漸變開始的顏色
	/*  crEnd:     漸變結束的顏色
	/*  iCenterpos: mode=1,2時有效，crEnd結束的位置（即顏色從crStart->crEnd->crStart,單位：百分比）
	/***************************************************************************************************/
	void FillVarColorRect( CDC *pDC, CRect &rect, int mode, COLORREF crStart, COLORREF crEnd, int iCenterPos);

	/***************************************************************************************************/
	/*	描述：畫漸變色過渡處理的背景色
	/*  mode: 0=只畫邊框.    1=從上至下漸變. 2=從左至右. 3=左上到右下.4=從中間向兩邊
	/*        (不畫邊框)  0x81=無邊框從上至下漸變. ... ...
	/*  crFrameLT: 邊框顏色(左上)
	/*  crFrameRB: 邊框顏色(右下)
	/*  crStart:   漸變開始的顏色
	/*  crEnd:     漸變結束的顏色
	/*  iCenterpos: mode=1,2時有效，crEnd結束的位置（即顏色從crStart->crEnd->crStart,單位：百分比）
	/***************************************************************************************************/
	void DrawRoundRect(CDC *pDC, CRect &rcFrame, int mode, COLORREF crFrameLT, COLORREF crFrameRB, COLORREF crStart, COLORREF crEnd, int iCenterPos=0);


	void DrawArrowBtn(CDC *pDC, CRect rect, DWORD dir, COLORREF cEdge1, COLORREF cEdge2, COLORREF cStart, COLORREF cEnd);
	void DrawCloseBtn(CDC *pDC, CRect rect, DWORD dir, COLORREF cEdge1, COLORREF cEdge2, COLORREF cStart, COLORREF cEnd);
	void DrawExpandBtn(CDC *pDC, CRect rect, BOOL bExpanded, COLORREF cEdge1, COLORREF cEdge2, COLORREF cStart, COLORREF cEnd);

	void DrawColorLine(CDC *pDC, int x1, int y1, int x2, int y2, COLORREF color, int linetype=PS_SOLID);
	void DrawColorLine( CDC *pDC, CPoint p1, CPoint p2, COLORREF color, int linetype=PS_SOLID);
	void DrawTriangle( CDC *pDC, CPoint p0, CPoint pt1, CPoint pt2, COLORREF color, BOOL bFill=TRUE );

	CSize GetStringSize( CDC *pDC, CString str );
};

#if 0
class DLL_API CMDC : public CDC 
{
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CMDC(CDC* pDC, const CRect& rcBounds);
	~CMDC();
	CMDC* operator->();
};
#endif

#undef DLL_API

#endif
