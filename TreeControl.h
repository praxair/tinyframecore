#pragma once
#include "framework.h"
#include "BaseControl.h"
#include "commctrl.h"

class CTreeControl: public CBaseControl
{
public:
	CTreeControl();
	~CTreeControl();

	virtual HWND Create(HWND hParent, int controlId, RECT &);

	virtual HTREEITEM InsertItem(const wchar_t* text, __int64 itemData, HTREEITEM hParent);
	virtual bool SetItem(HTREEITEM hItem, const wchar_t* text, __int64 itemData);
	virtual __int64 GetItemData(HTREEITEM hItem);
};

