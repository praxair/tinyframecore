#pragma once
#include "BaseControl.h"
class CEditControl :  public CBaseControl
{
public:
	CEditControl();
	~CEditControl();

	virtual HWND Create(HWND hParent, int controlId, RECT& rect);
	void SetText(const wchar_t*);
	void SelText(int start, int len);
	size_t GetLen();
private:

};

