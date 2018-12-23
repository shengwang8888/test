#pragma once

#include "lib\\DrawLib.h"

#ifdef _DEBUG
#pragma comment(lib,"lib\\Drawlib.dbg.lib")
#else
#pragma comment(lib,"lib\\Drawlib.rls.lib")
#endif

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

public:
	static void ClearAll();
	static CGP *GetGPLstHead();

	PDAILYINFO GetDailyInfoDate();


private:
	CString name;
	CString code;
	CString file_path;
	PDAILYINFO m_pDailyDate;

	static CGP *m_gpLstHead;
	CGP *m_gpNext;
	void Add_to_GpList();
	void Remove_from_GpList();

	BOOL LoadGPFile(LPCSTR strGpFile);
	void FreeGPFile();


};

enum ZOOMMODE {ZOOMRESET, ZOOMIN, ZOOMOUT};

class CGPMgr
{
public:
	CGPMgr();
	~CGPMgr();



	// for Draw UI
	int m_iScale;
	int m_iCellWidth;
	int m_iCellHigh;
	void UI_Zoom(ZOOMMODE mode);
	void Draw_GP_UI(CDC *pDC, CRect &rcMainWnd);

};

extern CGPMgr gpMgr;
