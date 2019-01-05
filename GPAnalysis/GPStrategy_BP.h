#pragma once
#include "GPStrategy.h"

// GPStrategy for Buy Point
class GPStrategy_BP : public GPStrategy
{
public:
	GPStrategy_BP();
	virtual ~GPStrategy_BP();

	virtual void getName(CString &name);
	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP);
	virtual GPStrategyReport *get_strategy_report();

private:
	CString m_strName;

	bool chkShouldBuy(PDAILYINFO pDayCurr);
	bool chkShouldSale(PDAILYINFO pDayCurr, PDAILYINFO pDayBuyed);
};

