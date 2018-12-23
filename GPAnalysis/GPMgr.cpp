#include "stdafx.h"
#include "GPMgr.h"

///////////////////////////////////////// CGP /////////////////////////////////////////
CGP *CGP::m_gpLstHead = 0;

CGP::CGP(LPCSTR strGpFile)
{
	name         = "";
	code         = "";
	file_path    = strGpFile;
	m_pDailyDate = NULL;

	if (LoadGPFile(strGpFile) == false)
		return;

	Add_to_GpList();


}

CGP::~CGP()
{
	Remove_from_GpList();
}

void CGP::Add_to_GpList()
{
	if (m_gpLstHead == 0)
	{
		m_gpLstHead = this;
		m_gpNext = NULL;
	}
	else
	{
		//CGP *pTmp = m_gpLstHead;
		//while (pTmp->m_gpNext) pTmp = pTmp->m_gpNext;
		//pTmp->m_gpNext = this;
		//m_gpNext = NULL;

		m_gpNext = m_gpLstHead;
		m_gpLstHead = this;

	}
}

void CGP::Remove_from_GpList()
{
	CGP *pTmp = m_gpLstHead;

	if (m_gpLstHead == NULL)
	{
		return;
	}
	else if (m_gpLstHead == this)
	{
		m_gpLstHead = m_gpLstHead->m_gpNext;
	}
	else
	{
		while (pTmp->m_gpNext != this && pTmp->m_gpNext != NULL)
		{
			pTmp = pTmp->m_gpNext;
		}

		if (pTmp->m_gpNext == this)
		{
			pTmp->m_gpNext = pTmp->m_gpNext->m_gpNext;
		}
	}

	FreeGPFile();
}


BOOL CGP::LoadGPFile(LPCSTR strGpFile)
{
	CFile fp;
	DAILYINFO dailyInfo;
	DAILYINFO *pHead;

	if (fp.Open(strGpFile, CFile::modeRead) == false)
	{
		AfxMessageBox((CString)"Error: fail to open file: " + strGpFile);
		return false;
	}

	if (m_pDailyDate != NULL)
		FreeGPFile();

	while (1)
	{
		if (fp.Read(&dailyInfo, 0x20) == 0x20)
		{
			DAILYINFO *pData = new DAILYINFO;

			memset(pData, 0, sizeof(DAILYINFO));
			memcpy(pData, &dailyInfo, 0x20);

			if (m_pDailyDate == NULL)
			{
				m_pDailyDate = pData;
				pHead = m_pDailyDate;
			}
			else
			{
				pHead->day_next = pData;
				pData->day_prev = pHead;

				pHead = pData;
			}

			m_pDailyDate->day_prev = pHead;
			//pHead->day_next = m_pDailyDate;
		}
		else
			break;
	}

	fp.Close();

	return true;
}

void CGP::FreeGPFile()
{
	DAILYINFO *pData = m_pDailyDate;

	while (m_pDailyDate != NULL)
	{
		pData = m_pDailyDate;
		m_pDailyDate = m_pDailyDate->day_next;

		delete pData;
	}
}

PDAILYINFO CGP::GetDailyInfoDate()
{
	return m_pDailyDate;
}

CGP *CGP::GetGPLstHead()
{
	return m_gpLstHead;
}

void CGP::ClearAll()
{
	while (m_gpLstHead)
	{
		CGP *pTmp = m_gpLstHead;

		delete pTmp;
	}

}



///////////////////////////////////////// GP Manager /////////////////////////////////////////
CGPMgr  gpMgr;
DrawLib drawLib;


CGPMgr::CGPMgr()
{
	m_iCellWidth = 10;
	m_iCellHigh  = 30;
}


CGPMgr::~CGPMgr()
{
	CGP::ClearAll();
}

void CGPMgr::UI_Zoom(ZOOMMODE mode)
{
	switch (mode)
	{
	case ZOOMRESET:
		m_iCellWidth = 10;
		break;
	case ZOOMIN:
		if (m_iCellWidth == 1)
			m_iCellWidth = 2;
		else
			m_iCellWidth += 2;
		break;
	case ZOOMOUT:
		m_iCellWidth -= 2;
		if (m_iCellWidth <= 0) m_iCellWidth = 1;
		break;

	}

}



void CGPMgr::Draw_GP_UI(CDC *pDC, CRect &rcMainWnd)
{
	CGP *pGP = CGP::GetGPLstHead();
	if (pGP == NULL) return;

	CRect rcGrain = rcMainWnd;
	CRect rcAMount = rcMainWnd;

	rcGrain.top = rcGrain.top + 20;
	rcGrain.bottom = (LONG)(rcGrain.bottom *0.7);

	rcAMount.top = rcGrain.bottom;

	//pDC->Draw3dRect(rcGrain, RGB(255, 0, 0), RGB(255, 0, 0));



	PDAILYINFO pDailyData = pGP->GetDailyInfoDate();
	if (pDailyData == NULL) return;

	int maxDay = rcGrain.Width() / m_iCellWidth;

	// Draw grain windows
	PDAILYINFO pTmpDat = pDailyData;
	int priceMin = 0x7fffffff;
	int priceMax = 0;
	int amountMax = 0;
	for (int i = 0; i < maxDay; i++)
	{
		pTmpDat = pTmpDat->day_prev;

		if (priceMin > pTmpDat->price_min)
			priceMin = pTmpDat->price_min;
		if (priceMax < pTmpDat->price_max)
			priceMax = pTmpDat->price_max;

		if (amountMax < pTmpDat->deal_amount)
			amountMax = pTmpDat->deal_amount;

		if (pTmpDat == pDailyData)
			break;
	}

	double hRatePrice = (double)(rcGrain.Height()) / (priceMax - priceMin);
	double hRateAMount = (double)(rcAMount.Height()) / amountMax;
	int dayCnt = 0;

	pTmpDat = pDailyData;
	do
	{
		dayCnt++;

		pTmpDat = pTmpDat->day_prev;

		int cell_X = rcGrain.right - dayCnt * m_iCellWidth;
		if (cell_X < 0) break;

		BOOL		bRising;
		COLORREF	dayColor;
		CRect		rcTmp;
		CPoint		pt1;
		CPoint		pt2;

		if (pTmpDat->price_close > pTmpDat->price_open)
		{
			bRising  = TRUE;

			dayColor = RGB(255, 0, 0);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice),
				cell_X + ((m_iCellWidth - 1) == 0 ? 1 : m_iCellWidth - 1),
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice));
		}
		else if(pTmpDat->price_close < pTmpDat->price_open)
		{
			bRising = FALSE;

			dayColor = RGB(84, 255, 255);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice),
				cell_X + ((m_iCellWidth - 1) == 0 ? 1 : m_iCellWidth - 1),
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice));
		}
		else
		{
			bRising = TRUE;

			dayColor = RGB(255, 50, 50);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice),
				cell_X + ((m_iCellWidth - 1) == 0 ? 1 : m_iCellWidth - 1),
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice - 1));
		}

		if (m_iCellWidth <= 2)
		{
			pt1 = CPoint(cell_X, rcGrain.bottom - (int)((pTmpDat->price_max - priceMin)*hRatePrice));
			pt2 = CPoint(cell_X, rcGrain.bottom - (int)((pTmpDat->price_min - priceMin)*hRatePrice));
		}
		else
		{
			pt1 = CPoint(cell_X + (m_iCellWidth - 1) / 2, rcGrain.bottom - (int)((pTmpDat->price_max - priceMin)*hRatePrice));
			pt2 = CPoint(cell_X + (m_iCellWidth - 1) / 2, rcGrain.bottom - (int)((pTmpDat->price_min - priceMin)*hRatePrice));
		}

		// Draw grain
		//drawLib.FillVarColorRect(pDC, rcTmp, 0x1, dayColor, dayColor, 0);
		drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);
		if (bRising)
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, RGB(0, 0, 0), 0);
		else
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, dayColor, 0);


		// Draw amount
		rcTmp = CRect(
			cell_X,
			rcAMount.bottom - (int)(pTmpDat->deal_amount*hRateAMount - 1),
			cell_X + ((m_iCellWidth - 1) == 0 ? 1 : m_iCellWidth - 1),
			rcAMount.bottom );

		if (bRising)
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, RGB(0, 0, 0), 0);
		else
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, dayColor, 0);


	} while (pTmpDat != pDailyData);



	//drawLib.DrawArrowBtn(pMDC, rcBtn, 0, 0, 0xffff, 0, 0xffff);
	//drawLib.FillVarColorRect( &memDC.GetDC(), rcBtn, 0x1, RGB(150, 150, 150), RGB(150, 150, 150), 0);
	//FillVarColorRect(pDC, rcZoom, 0x1, RGB(100, 100, 100), RGB(100, 100, 100), 0);
	//drawLib.DrawColorLine(pMDC, pt1, pt2, RGB(255, 255, 255), PS_SOLID);
}

