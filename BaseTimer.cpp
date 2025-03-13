#include "BaseTimer.h"

UINT_PTR CBaseTimer::timerId = 0;

CBaseTimer::CBaseTimer(CBaseWnd& wnd)
{
	init();
	this->wnd = &wnd;
}


CBaseTimer::~CBaseTimer()
{
	KillTimer();
}


void CBaseTimer::init()
{
	wnd = 0;
	timer = 0;
	interval = 0;
}


UINT_PTR CBaseTimer::SetTimer(UINT interval)
{
	if (wnd == 0)
		return false;

	if (!KillTimer())
		return 0;

	this->interval = interval;

	if (this->interval == 0)
		return 0;

	timer = ::SetTimer(wnd->GetHwnd(), ++timerId, this->interval, 0);

	return timer;
}


bool CBaseTimer::KillTimer()
{
	if (timer == 0)
		return true;

	if (::KillTimer(wnd->GetHwnd(), timerId))
		timer = 0;
}