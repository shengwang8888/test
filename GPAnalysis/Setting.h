#pragma once
#include "afxstr.h"


#define WM_USER_SHOW_GP WM_USER+100
#define WM_USER_ANALYSIS_SINGLE WM_USER+101

#define WM_USER_OUTPUTWND_MSG WM_USER+400


enum AVGLINE {
	LINE5, LINE10, LINE20, LINE30, LINE40, LINE50, LINE60, LINE120
};

typedef struct daily_info
{
	int date;
	int price_open;
	int price_max;
	int price_min;
	int price_close;
	int res0;
	int deal_amount;
	int res1;

	//CTime date;						// date.Format("%Y-%m-%d");

	struct daily_info *day_prev;		// day_prev is a infinite cycle
	struct daily_info *day_next;		// day_next end at NULL
	int avgLine[10];					// Normal is: 5, 10, 20, 30, 40, 50, 60 days average line
	int avgAmount[10];					// Normal is: 5, 10, 20, 30, 40, 50, 60 days average amount
	int profit;

}DAILYINFO, *PDAILYINFO;


class CSetting
{
public:
	CSetting();
	~CSetting();

public:
	CString m_DataSourcePath;

};

extern CSetting gSetting;
