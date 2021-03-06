#include "stdafx.h"
#include "GPStrategy.h"
#include "GPMgr.h"


GPStrategyReport::GPStrategyReport()
{

}
GPStrategyReport::~GPStrategyReport()
{
	Reset();
}

void GPStrategyReport::Reset()
{
	TRADEINFO *pTradeInfo;

	for (int i = 0; i < GetTradeTimes(); i++)
	{
		pTradeInfo = GetTradeInfo(i);
		if (pTradeInfo)
			delete pTradeInfo;
	}

	arTradeinfo.RemoveAll();
}

int GPStrategyReport::GetTradeTimes()
{
	return arTradeinfo.GetCount();
}

int GPStrategyReport::GetTotalProfit()
{
	TRADEINFO *pTradeInfo;
	int        ttp = 0;

	for (int i = 0; i < GetTradeTimes(); i++)
	{
		pTradeInfo = GetTradeInfo(i);
		if (pTradeInfo)
			ttp += pTradeInfo->Profit;
	}

	return ttp;
}

TRADEINFO *GPStrategyReport::GetTradeInfo(int idx)
{
	if (idx >= arTradeinfo.GetCount())
		return NULL;

	return (TRADEINFO *)arTradeinfo.GetAt(idx);
}

void GPStrategyReport::AddTradeInfo(TRADEINFO *pTradeInfo)
{
	arTradeinfo.Add(pTradeInfo);
}

int GPStrategyReport::GetWinTimes(int *iWinPercent)
{
	TRADEINFO *pTradeInfo;
	int        times = 0;
	int        precent = 0;


	for (int i = 0; i < GetTradeTimes(); i++)
	{
		pTradeInfo = GetTradeInfo(i);
		if (pTradeInfo)
		{
			if (pTradeInfo->Profit >= 0)
			{
				times++;
				precent += pTradeInfo->Profit;
			}
		}
	}

	if (iWinPercent) *iWinPercent = precent;
	return times;
}

int GPStrategyReport::GetLostTimes(int *iLostPercent)
{
	TRADEINFO *pTradeInfo;
	int        times = 0;
	int        precent = 0;


	for (int i = 0; i < GetTradeTimes(); i++)
	{
		pTradeInfo = GetTradeInfo(i);
		if (pTradeInfo)
		{
			if (pTradeInfo->Profit < 0)
			{
				times++;
				precent += pTradeInfo->Profit;
			}
		}
	}

	if (iLostPercent) *iLostPercent = precent;
	return times;
}




// ------------------------------------------------------------------------------------------

// Stragegy analysis 策略分析

GPStrategy::GPStrategy()
{
}


GPStrategy::~GPStrategy()
{
}




// ------------------------------------------------------------------------------------------
//(一)策略规则
//1.	收盘价位于60日均线以下的股票坚决不与理睬
//2.	收盘价突破60日均线时（一个点以上），尾盘买入
//3.	收盘价破位10日均线时，卖出
//4.	收盘价在60日均线之上，并且突破10日均线，（10日均线不超过60日均线20%）尾盘买入


GPStrategy_6010::GPStrategy_6010()
{
	m_strName = "GPStrategy_6010";
}


GPStrategy_6010::~GPStrategy_6010()
{
	m_report.Reset();
}

void GPStrategy_6010::getName(CString &name)
{
	name = m_strName;
}


bool GPStrategy_6010::do_strategy_analysis(int startDate, int endDate, CGP *pGP)
{
	PDAILYINFO	pDailyInfo, pDailyPrev;
	PTRADEINFO	pTradeInfo;
	bool		bBuyed = false;

	if (pGP == NULL) 
		return false;

	pDailyInfo = pGP->GetDailyInfoDate();
	pDailyPrev = NULL;

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
		pDailyInfo = pDailyInfo->day_next;
		if (pDailyInfo == NULL) return false;
	}

	// 02. calculate Strategy until to endDate
	while (endDate > pDailyInfo->date)
	{
		// 02.1 check should I buy in
		if (!bBuyed)
		{
			if (pDailyPrev != NULL)
			{
				if (pDailyPrev->price_close < pDailyPrev->avgLine[LINE60] &&
					pDailyInfo->price_close > pDailyPrev->avgLine[LINE60] * 1.01)
				{
					bBuyed = true;
					pTradeInfo = new TRADEINFO;
					pTradeInfo->BuyDate = pDailyInfo->date;
					pTradeInfo->BuyPrice = pDailyInfo->price_close;
				}
				else if (pDailyInfo->avgLine[LINE10] > pDailyInfo->avgLine[LINE60] &&
					pDailyPrev->price_close < pDailyPrev->avgLine[LINE10] && pDailyInfo->price_close > pDailyInfo->avgLine[LINE10] &&
					pDailyPrev->avgLine[LINE10] * 1.2 < pDailyPrev->avgLine[LINE60] &&
					pDailyInfo->price_close > pDailyInfo->avgLine[LINE60] * 1.01)
				{
					bBuyed = true;
					pTradeInfo = new TRADEINFO;
					pTradeInfo->BuyDate = pDailyInfo->date;
					pTradeInfo->BuyPrice = pDailyInfo->price_close;
				}
			}
		}


		// 02.1 check should I sale out
		if (bBuyed)
		{
			if (pDailyPrev != NULL && pTradeInfo->BuyDate != pDailyInfo->date)
			{
				if (pDailyInfo->price_close < pDailyPrev->avgLine[LINE10])
				{
					bBuyed = false;
					pTradeInfo->SaleDate = pDailyInfo->date;
					pTradeInfo->SalePrice = pDailyInfo->price_close;
					pTradeInfo->Profit = (int)((float)(pTradeInfo->SalePrice - pTradeInfo->BuyPrice) / pTradeInfo->BuyPrice * 10000); // 1x.xx%

					m_report.AddTradeInfo(pTradeInfo);
				}
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

GPStrategyReport *GPStrategy_6010::get_strategy_report()
{
	return &m_report;
}



///////////////////////////////////////////////////////////////////////////////////////////////////

GPStrategy_60P5::GPStrategy_60P5()
{
	m_strName = "GPStrategy_60P5";
}


GPStrategy_60P5::~GPStrategy_60P5()
{
	m_report.Reset();
}

void GPStrategy_60P5::getName(CString &name)
{
	name = m_strName;
}


bool GPStrategy_60P5::do_strategy_analysis(int startDate, int endDate, CGP *pGP)
{
	PDAILYINFO	pDailyInfo, pDailyPrev, pDailyBuy;
	PTRADEINFO	pTradeInfo;
	bool		bBuyed = false;

	if (pGP == NULL)
		return false;

	pDailyInfo = pGP->GetDailyInfoDate();
	pDailyPrev = NULL;

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
		if (pDailyInfo == NULL) return false;
	}

	// 02. calculate Strategy until to endDate
	while (endDate > pDailyInfo->date)
	{
		// 02.1 check should I buy in
		if (!bBuyed)
		{
			if (pDailyPrev != NULL && pDailyInfo->price_open != pDailyInfo->price_max)
			{
				if (pDailyPrev->avgAmount[LINE20] * 3 > pDailyInfo->deal_amount)
				{

					if ((pDailyPrev->price_close <= pDailyPrev->avgLine[LINE60] && pDailyInfo->price_close > pDailyPrev->avgLine[LINE60] ) ||
						(pDailyPrev->price_close < pDailyPrev->avgLine[LINE10] && pDailyInfo->price_close >= pDailyPrev->avgLine[LINE10] &&
							pDailyPrev->avgLine[LINE10] > pDailyPrev->avgLine[LINE60] && pDailyPrev->avgLine[LINE10] < pDailyPrev->avgLine[LINE60] * 1.12))
					{
						bBuyed = true;
						pDailyBuy = pDailyInfo;

						pTradeInfo = new TRADEINFO;
						pTradeInfo->BuyDate = pDailyInfo->date;
						pTradeInfo->BuyPrice = pDailyInfo->price_close;
					}
				}
			}
		}

		// 02.1 check should I sale out
		if (bBuyed)
		{
			if (pDailyPrev != NULL && pTradeInfo->BuyDate != pDailyInfo->date)
			{
				if ((pDailyInfo->price_close < pDailyPrev->avgLine[LINE60]) ||
					(pDailyPrev->price_close >= pDailyPrev->avgLine[LINE10] && pDailyInfo->price_close < pDailyPrev->avgLine[LINE10] && pDailyPrev->price_max > pDailyBuy->price_close * 1.3) ||
					(pDailyPrev->price_close >= pDailyPrev->avgLine[LINE5] && pDailyInfo->price_close < pDailyPrev->avgLine[LINE5] && pDailyPrev->price_max > pDailyBuy->price_close * 1.5) ||
					(pDailyPrev->avgAmount[LINE20] * 4 <= pDailyInfo->deal_amount) ||
					(pDailyInfo->profit < -500 && pDailyInfo->price_close < pDailyInfo->avgLine[LINE10]) )
				{
					bBuyed = false;

					pTradeInfo->SaleDate = pDailyInfo->date;
					pTradeInfo->SalePrice = pDailyInfo->price_close;
					pTradeInfo->Profit = (int)((float)(pTradeInfo->SalePrice - pTradeInfo->BuyPrice) / pTradeInfo->BuyPrice * 10000); // 1x.xx%

					m_report.AddTradeInfo(pTradeInfo);
				}
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

GPStrategyReport *GPStrategy_60P5::get_strategy_report()
{
	return &m_report;
}
