#pragma once


#include "GPStrategy.h"
#include "GPStrategy_bp.h"


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

	int AddStrategy(GPStrategy *pStrategy);
	void RemoveStrategy(GPStrategy *pStrategy);
	GPStrategy *GetStrategy(int index);
	int GetStrategyIdx(GPStrategy *pStrategy);

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

	CPtrArray m_arStrategy;
};

enum ZOOMMODE {ZOOMRESET, ZOOMIN, ZOOMOUT, SHIFTRESET, SHIFTLEFT, SHIFTRIGHT};
enum COLORPEN {
	PEN_WHITE, PEN_BLACK, PEN_60DAY, PEN_50DAY,
	PEN_40DAY, PEN_30DAY, PEN_20DAY, PEN_10DAY,
	PEN_5DAY, PEN_WIN, PEN_LOSE, PEN_WHITELINE,
	PEN_REDLINE, PEN_COORDINATEDOT, PEN_COORDINATE, PEN_TRADEWIN, 
	PEN_TRADELOSE, PEN_MAX
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
	void UI_MoveDayShift(int dayShift);
	void Draw_GP_UI(CDC *pDC, CRect &rcMainWnd, CPoint &ptMouse);

private:
	// for Draw UI
	int m_iScale;
	int m_iCellWidth;
	int m_iCellHigh;
	int m_iDayShift;			// for left/right key adjust day date display
	int m_iCellOverlap;


	CPen m_pens[PEN_MAX];
	COLORREF GetPenColor(COLORPEN penId);
	void DrawColorLine(CDC *pDC, CPoint p1, CPoint p2, COLORPEN penId = PEN_MAX);
	void FillColorRect(CDC *pDC, CRect &rect, COLORPEN pen1 = PEN_MAX, COLORPEN pen2 = PEN_MAX);
};

extern CGPMgr gpMgr;
