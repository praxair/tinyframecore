#include "TreeControl.h"

CTreeControl::CTreeControl()
{

}


CTreeControl::~CTreeControl()
{
    //Delete all items
    SendMessage(hThisControl, TVM_DELETEITEM, (WPARAM)0, (LPARAM)TVI_ROOT);
}


HWND CTreeControl::Create(HWND hParent, int controlId, RECT& rect)
{
    this->controlId = controlId;
    this->hParentWindow = hParent;

	hThisControl = CreateWindowEx(NULL,
                            WC_TREEVIEW,
                            TEXT("Tree View"),
                            WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
                            rect.left,
                            rect.top,
                            rect.right,
                            rect.bottom,
                            hParent,
                            (HMENU)controlId,
                            GetModuleHandle(NULL),
                            NULL);

    if (hThisControl == 0)
        MessageBox(NULL, L"Can not create tree view control", L"TreeView control", MB_OK | MB_ICONERROR);

    return hThisControl;
}


HTREEITEM CTreeControl::InsertItem(const wchar_t* text, __int64 itemData, HTREEITEM hParent)
{
    TVINSERTSTRUCT tvs = { 0 };

    tvs.hParent = hParent;
    tvs.hInsertAfter = TVI_LAST;
    tvs.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvs.item.pszText = (LPWSTR)text;
    tvs.item.lParam = itemData;

    return (HTREEITEM)SendMessage(hThisControl, TVM_INSERTITEM, (WPARAM)0, (LPARAM)&tvs);
}


bool CTreeControl::SetItem(HTREEITEM hItem,const wchar_t* text, __int64 itemData)
{
    TVITEMEX tvix = { 0 };

    tvix.mask = TVIF_PARAM;

    if (text != NULL)
        tvix.mask = tvix.mask | TVIF_TEXT;
    
    tvix.hItem = hItem;
    tvix.pszText = (LPWSTR)text;
    tvix.lParam = itemData;
    
    return (bool)SendMessage(hThisControl, TVM_SETITEM, (WPARAM)0, (LPARAM)&tvix);
}


__int64 CTreeControl::GetItemData(HTREEITEM hItem)
{
    TVITEMEX tvix = { 0 };

    tvix.mask = TVIF_PARAM;

    tvix.hItem = hItem;

    SendMessage(hThisControl, TVM_GETITEM, (WPARAM)0, (LPARAM)&tvix);

    return (__int64)tvix.lParam;
}