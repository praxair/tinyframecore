#pragma once
#include "BaseControl.h"
class CLabelControl :  public CBaseControl
{
public:
	CLabelControl();
	~CLabelControl();
	virtual HWND Create(HWND hParent, int controlId, RECT& rect);
	void SetText(const wchar_t*);
private:
	wchar_t* text;
};

