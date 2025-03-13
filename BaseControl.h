#pragma once
#include "framework.h"

class CBaseControl
{
public:
	CBaseControl();
	~CBaseControl();

	virtual HWND Create(HWND hParent, int controlId, RECT&) = 0;
	virtual void Show();
	virtual void Hide();
	virtual void Size(int x, int y, int width, int height);
	virtual void SetCaption (const wchar_t* cpation);
	virtual HWND GetHwnd() { return hThisControl; };
	virtual HWND GetParent() { return hParentWindow; };
	virtual __int64 GetControlId() { return controlId; };
	virtual const wchar_t* GetCaption() { return caption; };
protected:
	HWND hParentWindow;
	HWND hThisControl;
	wchar_t* caption;
	__int64  controlId;
};

