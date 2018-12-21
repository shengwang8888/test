#pragma once


typedef struct daily_info
{
	PDAILYINFO day_prev;
	PDAILYINFO day_next;
	CTime date;						// date.Format("%Y-%m-%d");
	int max;
	int min;
	//...
}DAILYINFO, *PDAILYINFO;

typedef struct gp_info
{
	CString name;
	CString code;
	CString file_path;
	PDAILYINFO daily_date;
}GPINFO, *PGPINFO;

class CGPMgr
{
public:
	CGPMgr();
	~CGPMgr();
};

