#pragma once
#include "atltime.h"

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

	struct daily_info *day_prev;
	struct daily_info *day_next;

}DAILYINFO, *PDAILYINFO;



class CGP
{
public:
	CGP(LPCSTR strGpFile);
	~CGP();
	BOOL LoadGPFile(LPCSTR strGpFile);
	void FreeGPFile();

public:
	static CGP *m_gpLstHead;
	static void ClearAll();


private:
	CString name;
	CString code;
	CString file_path;
	PDAILYINFO m_pDailyDate;

	CGP *m_gpNext;
	void Add_to_GpList();
	void Remove_from_GpList();

};

class CGPMgr
{
public:
	CGPMgr();
	~CGPMgr();

};

extern CGPMgr gpMgr;

