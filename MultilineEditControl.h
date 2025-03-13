#pragma once
#include "BaseControl.h"
class CMultilineEditControl : public CBaseControl
{
public:
	CMultilineEditControl();
	~CMultilineEditControl();

	virtual HWND Create(HWND hParent, int controlId, RECT& rect);
	void SetText(wchar_t*);
	wchar_t* GetText() { return text; };
private:
	wchar_t* text;

};

