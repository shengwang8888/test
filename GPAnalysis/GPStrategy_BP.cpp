#include "stdafx.h"
#include "GPStrategy_BP.h"
#include "GPMgr.h"


GPStrategy_BP::GPStrategy_BP()
{
	m_strName = "GPStrategy_BuyPoint";

}


GPStrategy_BP::~GPStrategy_BP()
{
	m_report.Reset();
}

void GPStrategy_BP::getName(CString &name)
{
	name =  m_strName;
}

GPStrategyReport *GPStrategy_BP::get_strategy_report()
{
	return &m_report;
}


bool GPStrategy_BP::do_strategy_analysis(int startDate, int endDate, CGP *pGP)
{
	PDAILYINFO	pDailyInfo, pDailyPrev, pDailyBuy;
	PTRADEINFO	pTradeInfo;
	bool		bBuyed = false;

	if (pGP == NULL)
		return false;

	pDailyInfo = pGP->GetDailyInfoDate();
	pDailyPrev = NULL;
	if (pDailyInfo == NULL)
		return false;


	if (startDate == 0)
		startDate = pDailyInfo->date;
	if (endDate == 0)
		endDate = pDailyInfo->day_prev->date;

	if (startDate > endDate)
		return false;

	m_report.Reset();

	// 01. find startDate
	while (startDate > pDailyInfo->date)
	{
		pDailyPrev = pDailyInfo;
		pDailyInfo = pDailyInfo->day_next;
		if (pDailyInfo == NULL)
			return false;
	}

	// 02. calculate Strategy until to endDate
	while (endDate > pDailyInfo->date)
	{
		if(pDailyPrev == NULL) 
		{
			pDailyPrev = pDailyInfo;
			pDailyInfo = pDailyInfo->day_next;
			if (pDailyInfo == NULL) break;

			continue;
		}

		if (!bBuyed)
		{
			if (chkShouldBuy(pDailyInfo))
			{
				bBuyed = true;
				pDailyBuy = pDailyInfo;

				pTradeInfo = new TRADEINFO;
				pTradeInfo->BuyDate = pDailyInfo->date;
				pTradeInfo->BuyPrice = pDailyInfo->price_close;
			}
		}
		else
		{
			if (chkShouldSale(pDailyInfo, pDailyBuy))
			{
				bBuyed = false;

				pTradeInfo->SaleDate = pDailyInfo->date;
				pTradeInfo->SalePrice = pDailyInfo->price_close;
				pTradeInfo->Profit = (int)((float)(pTradeInfo->SalePrice - pTradeInfo->BuyPrice) / pTradeInfo->BuyPrice * 10000); // 1x.xx%

				m_report.AddTradeInfo(pTradeInfo);
			}
		}

		pDailyPrev = pDailyInfo;
		pDailyInfo = pDailyInfo->day_next;
		if (pDailyInfo == NULL) break;
	}

	if (bBuyed)
	{
		bBuyed = false;
		pTradeInfo->SaleDate = pDailyPrev->date;
		pTradeInfo->SalePrice = pDailyPrev->price_close;
		pTradeInfo->Profit = (int)((float)(pTradeInfo->SalePrice - pTradeInfo->BuyPrice) / pTradeInfo->BuyPrice * 10000); // 1x.xx%

		m_report.AddTradeInfo(pTradeInfo);
	}

	return true;
}

bool GPStrategy_BP::chkShouldBuy(PDAILYINFO pDayCurr)
{
	PDAILYINFO pDayPrev = pDayCurr->day_prev;

	if (pDayPrev == NULL) return false;												// ��һ�죬ֻ��һ�����ݣ���������

	if (pDayCurr->price_open == pDayCurr->price_max) return false;					// ���з���ͣ���� �� ���ͣ����

	if (pDayCurr->deal_amount > pDayPrev->avgAmount[LINE20] * 3)					// β�̳ɽ�������ǰ5��ƽ���ɽ�����3��������
		return false;

	if ((pDayPrev->price_close <= pDayPrev->avgLine[LINE60] && pDayCurr->price_close > pDayPrev->avgLine[LINE60]) // ��������ͻ��60�վ���
		&& (pDayPrev->avgLine[LINE60] < pDayCurr->avgLine[LINE60] )				// 60�վ��߳�ˮƽ������̬��
		)
		return true;																




	return false;
}

bool GPStrategy_BP::chkShouldSale(PDAILYINFO pDayCurr, PDAILYINFO pDayBuyed)
{
	PDAILYINFO pDayPrev = pDayCurr->day_prev;

	if (pDayPrev == NULL) return false;

	//if (pDayCurr->profit < 0) return true;											// ֻҪ���µ��������������ڲ���������Ƿ�����
	return true;

	return false;
}





