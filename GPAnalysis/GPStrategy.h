#pragma once
#include "GPMgr.h"


typedef struct _TradeInfo
{
	int BuyDate;
	int BuyPrice;
	int SaleDate;
	int SalePrice;
	int Profit;					// Profit Lost Percent
}TRADEINFO, *PTRADEINFO;

class GPStrategyReport
{
public :
	GPStrategyReport();
	~GPStrategyReport();

	void Reset();
	int GetTradeTimes();
	int GetWinTimes(int *iWinPercent=0);
	int GetLostTimes(int *iLostPercent=0);
	int GetTotalProfit();
	TRADEINFO *GetTradeInfo(int idx);
	void AddTradeInfo(TRADEINFO *pTradeInfo);

private:
	CPtrArray arTradeinfo;
};





class GPStrategy
{
public:
	GPStrategy();
	~GPStrategy();

	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP) = 0;
	virtual GPStrategyReport *get_strategy_report() = 0;

protected:
	GPStrategyReport m_report;
};

class GPStrategy_6010 : public GPStrategy
{
public:
	GPStrategy_6010();
	~GPStrategy_6010();

	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP);
	virtual GPStrategyReport *get_strategy_report();

public:
	CString name;

};

