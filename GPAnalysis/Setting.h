#pragma once
#include "afxstr.h"


#define WM_USER_SHOW_GP WM_USER+100
#define WM_USER_ANALYSIS_SINGLE WM_USER+101

class CSetting
{
public:
	CSetting();
	~CSetting();

public:
	CString m_DataSourcePath;

};

extern CSetting gSetting;
