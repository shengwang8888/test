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
}


CGPMgr::~CGPMgr()
{
	CGP::ClearAll();
}

void CGPMgr::Draw_GP_UI(CDC *pDC, CRect &rcMainWnd)
{
	CGP *pGP = CGP::GetGPLstHead();
	if (pGP == NULL) return;

	PDAILYINFO pDailyData = pGP->GetDailyInfoDate();
	if (pDailyData == NULL) return;

	int dayCnt = 0;
	PDAILYINFO pTmpDat = pDailyData;
	do
	{
		dayCnt++;

		pTmpDat = pTmpDat->day_prev;

		int xStart = rcMainWnd.right - dayCnt * 10;
		if (xStart < 0) break;

		CRect      rcTmp(xStart, rcMainWnd.bottom-pTmpDat->price_max, xStart+8, rcMainWnd.bottom - pTmpDat->price_min);

		drawLib.FillVarColorRect(pDC, rcTmp, 0x1, RGB(255, 0, 0), RGB(255, 0, 0), 0);


	} while (pTmpDat != pDailyData);



	//drawLib.DrawArrowBtn(pMDC, rcBtn, 0, 0, 0xffff, 0, 0xffff);
	//drawLib.FillVarColorRect( &memDC.GetDC(), rcBtn, 0x1, RGB(150, 150, 150), RGB(150, 150, 150), 0);
	//FillVarColorRect(pDC, rcZoom, 0x1, RGB(100, 100, 100), RGB(100, 100, 100), 0);
	//drawLib.DrawColorLine(pMDC, pt1, pt2, RGB(255, 255, 255), PS_SOLID);
}

