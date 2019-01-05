#pragma once


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




#pragma warning(disable:4091)
extern class CGP;

class GPStrategy
{

public:
	GPStrategy();
	virtual ~GPStrategy();

	virtual void getName(CString &name) = 0;
	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP) = 0;
	virtual GPStrategyReport *get_strategy_report() = 0;

protected:
	GPStrategyReport m_report;
};



class GPStrategy_6010 : public GPStrategy
{
public:
	GPStrategy_6010();
	virtual ~GPStrategy_6010();

	virtual void getName(CString &name);
	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP);
	virtual GPStrategyReport *get_strategy_report();

private:
	CString m_strName;

};


class GPStrategy_60P5 : public GPStrategy
{
public:
	GPStrategy_60P5();
	virtual ~GPStrategy_60P5();

	virtual void getName(CString &name);
	virtual bool do_strategy_analysis(int startDate, int endDate, CGP *pGP);
	virtual GPStrategyReport *get_strategy_report();

private:
	CString m_strName;

};



