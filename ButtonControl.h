#pragma once
#include "BaseControl.h"
class CButtonControl : public CBaseControl
{
public:
	CButtonControl();
	~CButtonControl();
	
	virtual HWND Create(HWND hParent, int controlId, RECT& rect);
	void SetText(const wchar_t*);
	wchar_t* GetText() { return text; };
private:
	wchar_t* text;

};

