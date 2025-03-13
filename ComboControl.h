#pragma once
#include "framework.h"
#include "BaseControl.h"
#include "String.h"
#include "commctrl.h"


class CComboControl :  public CBaseControl
{
public:
	CComboControl();
	~CComboControl();

	virtual HWND Create(HWND hParent, int controlId, RECT& rect);

	virtual __int64 AddString(const wchar_t* text, __int64 itemData);
	virtual __int64 GetItemData(__int64 index);
	virtual __int64 GetCount();
	virtual bool Select(__int64 index);
	virtual __int64 GetSelected();
	virtual CString GetItem(__int64 index);
};

