#pragma once
#include <windows.h>

class CBaseApp
{
private:
	CBaseApp() = default;
public:
	static CBaseApp& GetApp() { static CBaseApp baseApp; return baseApp; }
	virtual int Run();
};

