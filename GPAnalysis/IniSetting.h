#pragma once
class CIniSetting
{
public:
	CIniSetting(void);
	~CIniSetting(void);

public:
	CString		strDataFolder;
};

extern CIniSetting gIniSetting;

