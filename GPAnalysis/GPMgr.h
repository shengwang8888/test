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

	struct daily_info *day_prev;		// day_prev is a infinite cycle
	struct daily_info *day_next;		// day_next end at NULL
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
	CGP *GetNextGP();

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
enum COLORPEN {
	PEN_WHITE, PEN_BLACK, PEN_60DAY, PEN_50DAY,
	PEN_40DAY, PEN_30DAY, PEN_20DAY, PEN_10DAY,
	PEN_5DAY, PEN_WIN, PEN_LOSE, PEN_WHITELINE,
	PEN_REDLINE, PEN_COORDINATEDOT, PEN_COORDINATE, PEN_MAX
};

typedef struct _pen_info
{
	int style;
	int width;
	COLORREF color;
}PEN_INFO, *PPEN_INFO;

class CGPMgr
{
public:
	CGPMgr();
	~CGPMgr();

	void UI_Zoom(ZOOMMODE mode);
	void Draw_GP_UI(CDC *pDC, CRect &rcMainWnd, CPoint &ptMouse);

private:
	// for Draw UI
	int m_iScale;
	int m_iCellWidth;
	int m_iCellHigh;

	CPen m_pens[PEN_MAX];
	COLORREF GetPenColor(COLORPEN penId);
	void DrawColorLine(CDC *pDC, CPoint p1, CPoint p2, COLORPEN penId = PEN_MAX);
	void FillColorRect(CDC *pDC, CRect &rect, COLORPEN pen1 = PEN_MAX, COLORPEN pen2 = PEN_MAX);
};

extern CGPMgr gpMgr;
