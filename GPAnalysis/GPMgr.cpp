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
	CFile     fp;
	DAILYINFO dailyInfo;
	DAILYINFO *pHead;
	int       dayVal[120];
	int       maxDay = sizeof(dayVal) / sizeof(dayVal[0]);
	int       dayCnt = 0;
	int       i;

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


			// calculate average line data
			if (dayCnt == 0)
			{
				// init all data
				for (i = 0; i < maxDay; i++) dayVal[i] = pData->price_close;
			}

			dayVal[dayCnt%maxDay] = pData->price_close;

			for (i = 0; i < maxDay; i++)
			{
				int datPos = (dayCnt % maxDay) - i;

				if (datPos < 0) datPos += maxDay;

				if (i < 5)  pData->avgLine[LINE5] += dayVal[datPos];
				if (i < 10) pData->avgLine[LINE10] += dayVal[datPos];
				if (i < 20) pData->avgLine[LINE20] += dayVal[datPos];
				if (i < 30) pData->avgLine[LINE30] += dayVal[datPos];
				if (i < 40) pData->avgLine[LINE40] += dayVal[datPos];
				if (i < 50) pData->avgLine[LINE50] += dayVal[datPos];
				if (i < 60) pData->avgLine[LINE60] += dayVal[datPos];
				if (i < 120) pData->avgLine[LINE120] += dayVal[datPos];
			}

			pData->avgLine[LINE5] /= 5;
			pData->avgLine[LINE10] /= 10;
			pData->avgLine[LINE20] /= 20;
			pData->avgLine[LINE30] /= 30;
			pData->avgLine[LINE40] /= 40;
			pData->avgLine[LINE50] /= 50;
			pData->avgLine[LINE60] /= 60;
			pData->avgLine[LINE120] /= 120;

			dayCnt++;
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
		m_iScale = 0;
		break;
	case ZOOMIN:
		m_iScale++;
		break;
	case ZOOMOUT:
		m_iScale--;
		break;
	}
}



void CGPMgr::Draw_GP_UI(CDC *pDC, CRect &rcMainWnd, CPoint &ptMouse)
{
	CGP *pGP = CGP::GetGPLstHead();
	if (pGP == NULL) return;

	CRect rcGrain = rcMainWnd;
	CRect rcAMount = rcMainWnd;
	CRect rcPriceLeft = rcMainWnd;
	CRect rcDate = rcMainWnd;

	rcGrain.top = rcGrain.top + 10;
	rcGrain.right = rcGrain.right - 50;
	rcGrain.bottom = (LONG)(rcGrain.bottom *0.7);

	rcPriceLeft.left = rcGrain.right;
	rcPriceLeft.bottom = rcGrain.bottom;

	rcAMount.top = rcGrain.bottom + 10;
	rcAMount.bottom = rcMainWnd.bottom - 18;

	rcDate.top = rcAMount.bottom +2;



	PDAILYINFO pDailyData = pGP->GetDailyInfoDate();
	if (pDailyData == NULL) return;

	int cell_overlap = 0;
	int cell_W = m_iCellWidth + m_iScale * 2;
	if (cell_W <= 0)
	{
		cell_W = 1;
		cell_overlap = -(m_iCellWidth + m_iScale * 2) / 2;
	}

	int maxDay = rcGrain.Width() / cell_W;
	if (cell_overlap > 0)
		maxDay *= cell_overlap+1;

	// Draw grain windows
	PDAILYINFO pSelectedDay = NULL;
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
	int overlapCnt = 0;
	int lastDate = 0;

	CString		strDate;
	CString     strPrice;
	BOOL		bRising;
	COLORREF	dayColor;
	COLORREF	whiteLineColor = RGB(192, 192, 192);
	COLORREF	L60Color = RGB(0, 192, 0);

	CRect		rcTmp;
	CPoint		pt1;
	CPoint		pt2;

	// Draw_Coordinate
	int priceStep = (priceMax - priceMin) / 20;

	dayColor = RGB(176, 0, 0);
	pDC->SetTextColor(dayColor);
	pDC->SetBkMode(TRANSPARENT);

	pt1 = CPoint(rcMainWnd.left, rcGrain.bottom);
	pt2 = CPoint(rcMainWnd.right, rcGrain.bottom);
	drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);
	pt1 = CPoint(rcGrain.right, rcMainWnd.top);
	pt2 = CPoint(rcGrain.right, rcMainWnd.bottom);
	drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);
	pt1 = CPoint(rcDate.left, rcDate.top);
	pt2 = CPoint(rcDate.right, rcDate.top);
	drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);

	for (int i = 1; i < 20; i++)
	{
		int yPos = rcGrain.bottom - (int)((priceStep * i)*hRatePrice);
		pt1 = CPoint(rcGrain.right, yPos);
		pt2 = CPoint(rcGrain.right+4, yPos);
		drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);


		if (i % 2 == 0)
		{
			pt1 = CPoint(rcGrain.left, yPos);
			pt2 = CPoint(rcGrain.right, yPos);
			drawLib.DrawColorLine(pDC, pt1, pt2, dayColor, PS_DOT);

			strPrice.Format("%.2f", (double)(priceMin+ priceStep*i)/100);
			pDC->TextOutA(rcGrain.right + 6, yPos - 8, strPrice);
			//pDC->ExtTextOutA( )
		}

	}

	// Draw grain
	int cell_X = rcGrain.left;
	int last_cell_X;
	do
	{
		last_cell_X = cell_X;

		cell_X = rcGrain.left + dayCnt * cell_W;
		if (cell_X < 0) break;

		if (cell_overlap > 0)
		{
			if (overlapCnt == cell_overlap)
			{
				overlapCnt = 0;
				dayCnt++;
			}
			else
				overlapCnt++;
		}
		else
			dayCnt++;

		if (pTmpDat->price_close > pTmpDat->price_open)
		{
			bRising  = TRUE;

			dayColor = RGB(255, 50, 50);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice),
				cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice));
		}
		else if(pTmpDat->price_close < pTmpDat->price_open)
		{
			bRising = FALSE;

			dayColor = RGB(84, 255, 255);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice),
				cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice));
		}
		else
		{
			bRising = TRUE;

			dayColor = RGB(255, 50, 50);
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice),
				cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice - 1));
		}

		if (cell_W <= 2)
		{
			pt1 = CPoint(cell_X, rcGrain.bottom - (int)((pTmpDat->price_max - priceMin)*hRatePrice));
			pt2 = CPoint(cell_X, rcGrain.bottom - (int)((pTmpDat->price_min - priceMin)*hRatePrice));
		}
		else
		{
			pt1 = CPoint(cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((pTmpDat->price_max - priceMin)*hRatePrice));
			pt2 = CPoint(cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((pTmpDat->price_min - priceMin)*hRatePrice));
		}

		drawLib.DrawColorLine(pDC, pt1, pt2, dayColor);
		if (bRising)
			drawLib.FillVarColorRect(pDC, rcTmp, 0x1, dayColor, RGB(80, 0, 20), 0);
		else
			drawLib.FillVarColorRect(pDC, rcTmp, 0x1, dayColor, RGB(50, 80, 80), 0);


		// Draw amount
		rcTmp = CRect(
			cell_X,
			rcAMount.bottom - (int)(pTmpDat->deal_amount*hRateAMount - 1),
			cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
			rcAMount.bottom );

		if (bRising)
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, RGB(0, 0, 0), 0);
		else
			drawLib.FillVarColorRect(pDC, rcTmp, 0x5, dayColor, dayColor, 0);


		// For draw fix date/price
		if (ptMouse.x >= rcTmp.left && ptMouse.x <= rcTmp.right)
		{
			pSelectedDay = pTmpDat;
		}

		
		// Draw average lines
		if (cell_X > rcGrain.left)
		{
			int avg0 = pTmpDat->day_prev->avgLine[LINE60];
			int avg1 = pTmpDat->avgLine[LINE60];

			pt1 = CPoint(cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg1 - priceMin)*hRatePrice));
			pt2 = CPoint(last_cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg0 - priceMin)*hRatePrice));

			if( rcGrain.PtInRect(pt1) && rcGrain.PtInRect(pt2))
				drawLib.DrawColorLine(pDC, pt1, pt2, L60Color);
		}



		pTmpDat = pTmpDat->day_next;

	} while (pTmpDat != pDailyData && pTmpDat != NULL );



	// Draw mouse line
	pt1 = CPoint(ptMouse.x, rcMainWnd.top);
	pt2 = CPoint(ptMouse.x, rcMainWnd.bottom);
	drawLib.DrawColorLine(pDC, pt1, pt2, whiteLineColor);

	pt1 = CPoint(rcMainWnd.left, ptMouse.y);
	pt2 = CPoint(rcMainWnd.right, ptMouse.y);
	drawLib.DrawColorLine(pDC, pt1, pt2, whiteLineColor);


	// Draw dynamic date/price
	pDC->SetBkMode(OPAQUE);
	pDC->SetBkColor(RGB(0, 0, 128));
	pDC->SetTextColor(whiteLineColor);

	if (pSelectedDay)
	{
		strDate.Format("%d/%d/%d", pSelectedDay->date / 10000, (pSelectedDay->date / 100) % 100, pSelectedDay->date % 100);
		pDC->TextOutA(ptMouse.x, rcDate.top, strDate);
	}

	if (rcGrain.PtInRect(ptMouse))
	{
		strPrice.Format("%.2f", ((double)(rcGrain.bottom - ptMouse.y) / hRatePrice + priceMin) / 100);
		pDC->TextOutA(rcGrain.right + 6, ptMouse.y - 8, strPrice);
	}

}


