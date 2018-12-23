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
	/*	���������u׃ɫ�^��̎��ı���ɫ
	/*  mode: 0=ֻ��߅��. 1=�������u׃. 2=��������. 3=���ϵ�����.4=�����g���߅(����)
	/*  crStart:   �u׃�_ʼ���ɫ
	/*  crEnd:     �u׃�Y�����ɫ
	/*  iCenterpos: mode=1,2�r��Ч��crEnd�Y����λ�ã����ɫ��crStart->crEnd->crStart,��λ���ٷֱȣ�
	/***************************************************************************************************/
	void FillVarColorRect( CDC *pDC, CRect &rect, int mode, COLORREF crStart, COLORREF crEnd, int iCenterPos);

	/***************************************************************************************************/
	/*	���������u׃ɫ�^��̎��ı���ɫ
	/*  mode: 0=ֻ��߅��.    1=�������u׃. 2=��������. 3=���ϵ�����.4=�����g���߅
	/*        (����߅��)  0x81=�o߅��������u׃. ... ...
	/*  crFrameLT: ߅���ɫ(����)
	/*  crFrameRB: ߅���ɫ(����)
	/*  crStart:   �u׃�_ʼ���ɫ
	/*  crEnd:     �u׃�Y�����ɫ
	/*  iCenterpos: mode=1,2�r��Ч��crEnd�Y����λ�ã����ɫ��crStart->crEnd->crStart,��λ���ٷֱȣ�
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
