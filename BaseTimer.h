#pragma once
#include "framework.h"
#include "BaseWnd.h"

class CBaseTimer
{
public:
	CBaseTimer() { };
	CBaseTimer(CBaseWnd& wnd);
	~CBaseTimer();

	void SetWindow(CBaseWnd& wnd) { this->wnd = &wnd; };
	CBaseWnd& GetHwnd() { return *wnd; }

	UINT_PTR SetTimer(UINT interval);
	bool KillTimer();

	UINT_PTR GetId() { return timerId; }
	UINT_PTR GetInterval() { return interval; }
	bool IsSet() { return (timer == 0) ? false : true; }
private:
	static UINT_PTR timerId;
	
	CBaseWnd* wnd;
	
	UINT_PTR timer;
	UINT interval;

	void init();
};