#pragma once

#include "lib\\DrawLib.h"

#ifdef _DEBUG
#pragma comment(lib,"lib\\Drawlib.dbg.lib")
#else
#pragma comment(lib,"lib\\Drawlib.rls.lib")
#endif

enum AVGLINE { LINE5, LINE10, LINE20, LINE30, LINE40, LINE50, LINE60, LINE120
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

	struct daily_info *day_prev;
	struct daily_info *day_next;
	int avgLine[10];					// Normal is: 5, 10, 20, 30, 40, 50, 60 days average line


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
	void Draw_GP_UI(CDC *pDC, CRect &rcMainWnd, CPoint &ptMouse);

};

extern CGPMgr gpMgr;
