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
				pHead->day_prev = m_pDailyDate;
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

CGP *CGP::GetNextGP()
{
	return m_gpNext;
}

void CGP::ClearAll()
{
	while (m_gpLstHead)
	{
		CGP *pTmp = m_gpLstHead;

		delete pTmp;
	}

}


int CGP::AddStrategy(GPStrategy *pStrategy)
{
	int ret = -1;
	if (pStrategy == NULL) return ret;

	m_arStrategy.Add(pStrategy);

	ret = m_arStrategy.GetCount() - 1;

	return ret;
}

void CGP::RemoveStrategy(GPStrategy *pStrategy)
{
	for (int i = 0; i < m_arStrategy.GetCount(); i++)
	{
		if (m_arStrategy.GetAt(i) == (void *)pStrategy)
		{
			m_arStrategy.RemoveAt(i);
		}
	}

}

GPStrategy *CGP::GetStrategy(int index)
{
	if (index >= m_arStrategy.GetCount())
		return 0;

	return (GPStrategy *)m_arStrategy.GetAt(index);
}

int CGP::GetStrategyIdx(GPStrategy *pStrategy)
{
	for (int i = 0; i < m_arStrategy.GetCount(); i++)
	{
		if (m_arStrategy.GetAt(i) == (void *)pStrategy)
			return i;
	}

	return -1;
}


///////////////////////////////////////// GP Manager /////////////////////////////////////////
CGPMgr  gpMgr;
//DrawLib drawLib;

//enum COLORPEN {
//	PEN_WHITE, PEN_BLACK, PEN_60DAY, PEN_50DAY,
//	PEN_40DAY, PEN_30DAY, PEN_20DAY, PEN_10DAY,
//	PEN_5DAY, PEN_WIN, PEN_LOSE, PEN_WHITELINE,
//	PEN_REDLINE, PEN_COORDINATEDOT, PEN_COORDINATE, PEN_TRADEWIN, 
//  PEN_TRADELOSE, PEN_MAX
//};


PEN_INFO penInfo[] = {
	{PS_SOLID, 1, RGB(255, 255, 255)},
	{PS_SOLID, 1, RGB(0, 0, 0)},
	{PS_SOLID, 1, RGB(0, 192, 0)},				//60
	{PS_SOLID, 1, RGB(30, 192, 0)},

	{PS_SOLID, 1, RGB(30, 192, 30)},
	{PS_SOLID, 1, RGB(60, 192, 0)},
	{PS_SOLID, 1, RGB(60, 192, 60)},
	{PS_SOLID, 1, RGB(192, 192, 0)},			//10

	{PS_SOLID, 1, RGB(192, 192, 192)},			//5
	{PS_SOLID, 1, RGB(255, 50, 50)},			//win
	{PS_SOLID, 1, RGB(84, 255, 255)},			//lost
	{PS_SOLID, 1, RGB(192, 192, 192)},

	{PS_SOLID, 1, RGB(255, 50, 50)},
	{PS_DOT,   1, RGB(176, 0, 0)},
	{PS_SOLID, 1, RGB(176, 0, 0)},
	{PS_SOLID, 2, RGB(136, 50, 50)},			// trade win

	{PS_SOLID, 2, RGB(110, 150, 190)},			// trade lost
};


CGPMgr::CGPMgr()
{
	m_iCellWidth = 10;
	m_iCellHigh  = 30;
	m_iDayShift  = 0;
	m_iCellOverlap = 0;


	for (int i = 0; i < PEN_MAX; i++)
	{
		m_pens[i].CreatePen(penInfo[i].style, penInfo[i].width, penInfo[i].color );
	}
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

	case SHIFTRESET:
		m_iDayShift = 0;
		break;
	case SHIFTLEFT:
		m_iDayShift+= m_iCellOverlap+1;
		break;
	case SHIFTRIGHT:
		m_iDayShift-= m_iCellOverlap+1;
		if (m_iDayShift < 0) m_iDayShift = 0;
		break;
	}
}

void CGPMgr::UI_MoveDayShift(int dayShift)
{
	m_iDayShift += dayShift * (m_iCellOverlap + 1);

	if (m_iDayShift < 0)
		m_iDayShift = 0;
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

	PTRADEINFO        pTradeInfo = NULL;
	PTRADEINFO        pTradeTemp = NULL;
	bool              bTradeArea = false;

	GPStrategyReport *pStrategyReport = NULL;
	GPStrategy       *pStrategy       = pGP->GetStrategy(0);
	if(pStrategy)
		pStrategyReport = pStrategy->get_strategy_report();

	int cell_W = m_iCellWidth + m_iScale * 2;
	if (cell_W <= 0)
	{
		cell_W = 1;
		m_iCellOverlap = -(m_iCellWidth + m_iScale * 2) / 2;
	}

	int maxDay = rcGrain.Width() / cell_W;
	if (m_iCellOverlap > 0)
		maxDay *= m_iCellOverlap+1;

	// Draw grain windows
	PDAILYINFO pSelectedDay = NULL;
	PDAILYINFO pTmpDat = pDailyData;
	for (int i = 0; i < maxDay + m_iDayShift; i++)
	{
		pTmpDat = pTmpDat->day_prev;

		if (pTmpDat == pDailyData)
		{
			if (i < maxDay)
				m_iDayShift = 0;
			else
				m_iDayShift = i - maxDay;

			break;
		}
	}
	pTmpDat = pDailyData;

	int priceMin = 0x7fffffff;
	int priceMax = 0;
	int amountMax = 0;
	for (int i = 0; i < maxDay + m_iDayShift; i++)
	{
		pTmpDat = pTmpDat->day_prev;
		if (i < m_iDayShift) continue;

		if (priceMin > pTmpDat->price_min)
			priceMin = pTmpDat->price_min;
		if (priceMax < pTmpDat->price_max)
			priceMax = pTmpDat->price_max;

		if (amountMax < pTmpDat->deal_amount)
			amountMax = pTmpDat->deal_amount;

		if (pTmpDat == pDailyData)
		{
			if (i < maxDay)
				m_iDayShift = 0;
			else
				m_iDayShift = i - maxDay;

			break;
		}
	}

	double hRatePrice = (double)(rcGrain.Height()) / (priceMax - priceMin);
	double hRateAMount = (double)(rcAMount.Height()) / amountMax;
	int overlapCnt = 0;
	int lastDate = 0;

	CString		strDate;
	CString     strPrice;
	BOOL		bRising;
	COLORPEN	penColor;
	COLORREF	whiteLineColor = RGB(192, 192, 192);
	COLORREF	L60Color = RGB(0, 192, 0);
	COLORREF	L10Color = RGB(192, 192, 0);

	CRect		rcTmp;
	CPoint		pt1;
	CPoint		pt2;

	// Draw_Coordinate
	int priceStep = (priceMax - priceMin) / 20;

	penColor = PEN_COORDINATE;
	pDC->SetTextColor(penInfo[penColor].color);
	pDC->SetBkMode(TRANSPARENT);

	pt1 = CPoint(rcMainWnd.left, rcGrain.bottom);
	pt2 = CPoint(rcMainWnd.right, rcGrain.bottom);
	DrawColorLine(pDC, pt1, pt2, penColor);
	pt1 = CPoint(rcGrain.right, rcMainWnd.top);
	pt2 = CPoint(rcGrain.right, rcMainWnd.bottom);
	DrawColorLine(pDC, pt1, pt2);
	pt1 = CPoint(rcDate.left, rcDate.top);
	pt2 = CPoint(rcDate.right, rcDate.top);
	DrawColorLine(pDC, pt1, pt2);

	for (int i = 1; i < 20; i++)
	{
		int yPos = rcGrain.bottom - (int)((priceStep * i)*hRatePrice);
		pt1 = CPoint(rcGrain.right, yPos);
		pt2 = CPoint(rcGrain.right+4, yPos);
		DrawColorLine(pDC, pt1, pt2, penColor);


		if (i % 2 == 0)
		{
			pt1 = CPoint(rcGrain.left, yPos);
			pt2 = CPoint(rcGrain.right, yPos);
			DrawColorLine(pDC, pt1, pt2, PEN_COORDINATEDOT);

			strPrice.Format("%.2f", (double)(priceMin+ priceStep*i)/100);
			pDC->TextOutA(rcGrain.right + 6, yPos - 8, strPrice);
			//pDC->ExtTextOutA( )
		}

	}

	
	int cell_X = rcGrain.left;
	int last_cell_X;
	int dayCnt = 0;

	do
	{
		last_cell_X = cell_X;

		cell_X = rcGrain.left + dayCnt * cell_W;
		if (cell_X < 0 || cell_X+ cell_W > rcGrain.right) break;

		if (m_iCellOverlap > 0)
		{
			if (overlapCnt == m_iCellOverlap)
			{
				overlapCnt = 0;
				dayCnt++;
			}
			else
				overlapCnt++;
		}
		else
			dayCnt++;


		// Draw strategy trade area
		#pragma region Draw_Trade_area

		if (pStrategyReport)
		{
			static CPoint ptTradeStart;

			for (int i = 0; i < pStrategyReport->GetTradeTimes(); i++)
			{
				pTradeTemp = pStrategyReport->GetTradeInfo(i);

				if (pTradeTemp->BuyDate == pTmpDat->date)
				{
					bTradeArea = true;
					pTradeInfo = pTradeTemp;
					ptTradeStart.x = cell_X;
					ptTradeStart.y = rcGrain.top;
					break;
				}
				if (pTradeTemp->SaleDate == pTmpDat->date)
				{
					if (bTradeArea)
					{
						CString str;

						if (pTradeInfo->Profit > 0)
							pDC->SetTextColor(GetPenColor(PEN_TRADELOSE));
						else
							pDC->SetTextColor(GetPenColor(PEN_TRADEWIN));

						str.Format("%d, buy: %.2f", pTradeInfo->BuyDate, (float)pTradeInfo->BuyPrice / 100);
						pDC->TextOutA(ptTradeStart.x, ptTradeStart.y, str);
						str.Format("%d, sale:%.2f", pTradeInfo->SaleDate, (float)pTradeInfo->SalePrice / 100);
						pDC->TextOutA(ptTradeStart.x, ptTradeStart.y+12, str);
						str.Format("profit: %.2f%%", (float)pTradeInfo->Profit / 100);
						pDC->TextOutA(ptTradeStart.x, ptTradeStart.y+24, str);
					}

					bTradeArea = false;

					break;
				}
			}

			if (bTradeArea)
			{
				if (pTradeInfo->Profit > 0)
					penColor = PEN_TRADEWIN;
				else
					penColor = PEN_TRADELOSE;

				rcTmp = CRect(
					cell_X,
					rcGrain.top,
					cell_X + cell_W,
					rcGrain.bottom-1);

				FillColorRect(pDC, rcTmp, penColor, penColor);
			}


		}
		#pragma endregion


		if (pTmpDat->price_close > pTmpDat->price_open)
		{
			bRising  = TRUE;

			penColor = PEN_WIN;
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice),
				cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice));
		}
		else if(pTmpDat->price_close < pTmpDat->price_open)
		{
			bRising = FALSE;

			penColor = PEN_LOSE;
			rcTmp = CRect(
				cell_X,
				rcGrain.bottom - (int)((pTmpDat->price_open - priceMin)*hRatePrice),
				cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
				rcGrain.bottom - (int)((pTmpDat->price_close - priceMin)*hRatePrice));
		}
		else
		{
			bRising = TRUE;

			penColor = PEN_WIN;
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

		// Draw grain
		DrawColorLine(pDC, pt1, pt2, penColor);
		if (bRising)
			FillColorRect(pDC, rcTmp, penColor, PEN_BLACK);
		else
			FillColorRect(pDC, rcTmp, penColor, penColor);


		// Draw amount
		rcTmp = CRect(
			cell_X,
			rcAMount.bottom - (int)(pTmpDat->deal_amount*hRateAMount - 1),
			cell_X + ((cell_W - 1) == 0 ? 1 : cell_W - 1),
			rcAMount.bottom );

		if (bRising)
			FillColorRect(pDC, rcTmp, penColor, PEN_BLACK);
		else
			FillColorRect(pDC, rcTmp, penColor, penColor);


		// For draw fix date/price
		if (ptMouse.x >= rcTmp.left && ptMouse.x <= rcTmp.right)
		{
			pSelectedDay = pTmpDat;
		}

		
		// Draw average lines
		if (cell_X > rcGrain.left)
		{
			// Draw 60 days average line
			int avg0 = pTmpDat->day_prev->avgLine[LINE60];
			int avg1 = pTmpDat->avgLine[LINE60];

			pt1 = CPoint(cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg1 - priceMin)*hRatePrice));
			pt2 = CPoint(last_cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg0 - priceMin)*hRatePrice));

			if( rcGrain.PtInRect(pt1) && rcGrain.PtInRect(pt2))
				DrawColorLine(pDC, pt1, pt2, PEN_60DAY);


			// Draw 60 days average line
			avg0 = pTmpDat->day_prev->avgLine[LINE10];
			avg1 = pTmpDat->avgLine[LINE10];

			pt1 = CPoint(cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg1 - priceMin)*hRatePrice));
			pt2 = CPoint(last_cell_X + (cell_W - 1) / 2, rcGrain.bottom - (int)((avg0 - priceMin)*hRatePrice));

			if (rcGrain.PtInRect(pt1) && rcGrain.PtInRect(pt2))
				DrawColorLine(pDC, pt1, pt2, PEN_10DAY);

		}



		pTmpDat = pTmpDat->day_next;

	} while (pTmpDat != pDailyData && pTmpDat != NULL );



	// Draw mouse line
	pt1 = CPoint(ptMouse.x, rcMainWnd.top);
	pt2 = CPoint(ptMouse.x, rcMainWnd.bottom);
	DrawColorLine(pDC, pt1, pt2, PEN_WHITELINE);

	pt1 = CPoint(rcMainWnd.left, ptMouse.y);
	pt2 = CPoint(rcMainWnd.right, ptMouse.y);
	DrawColorLine(pDC, pt1, pt2);


	// Draw dynamic date/price
	pDC->SetBkMode(OPAQUE);
	pDC->SetBkColor(RGB(0, 0, 128));
	pDC->SetTextColor(GetPenColor(PEN_WHITELINE));

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

void CGPMgr::DrawColorLine(CDC *pDC, CPoint p1, CPoint p2, COLORPEN penId)
{
	if (penId > PEN_MAX) 
		return;

	if( penId < PEN_MAX )
		pDC->SelectObject(m_pens[penId]);

	pDC->MoveTo(p1.x, p1.y);
	pDC->LineTo(p2.x, p2.y);
}

void CGPMgr::FillColorRect(CDC *pDC, CRect &rect, COLORPEN pen1, COLORPEN pen2)
{
	if (pen1 > PEN_MAX || pen2 > PEN_MAX) return;

	if (rect.Width() <= 0 || rect.Height() <= 0) return;

	if (pen2 < PEN_MAX)
		pDC->SelectObject(m_pens[pen2]);


	for (int i = 0; i < rect.Width(); i++)
	{
		pDC->MoveTo(rect.left+i, rect.top);
		pDC->LineTo(rect.left+i, rect.bottom-1);
	}

	if (pen1 < PEN_MAX)
		pDC->SelectObject(m_pens[pen1]);

	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom-1);

	if (rect.Width() > 1)
	{
		pDC->LineTo(rect.right-1, rect.bottom-1);
		pDC->LineTo(rect.right-1, rect.top);
		pDC->LineTo(rect.left, rect.top);
	}
}

COLORREF CGPMgr::GetPenColor(COLORPEN penId)
{
	if (penId >= PEN_MAX) return 0;

	return penInfo[penId].color;
}




