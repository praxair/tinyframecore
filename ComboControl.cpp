#include "ComboControl.h"

CComboControl::CComboControl()
{}

CComboControl::~CComboControl()
{}


HWND CComboControl::Create(HWND hParent, int controlId, RECT& rect)
{
    this->controlId = controlId;
    this->hParentWindow = hParent;

    hThisControl = CreateWindowEx(NULL,
        WC_COMBOBOX,
        TEXT("ComboBox"),
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
        rect.left,
        rect.top,
        rect.right,
        rect.bottom,
        hParent,
        (HMENU)controlId,
        GetModuleHandle(NULL),
        NULL);

    if (hThisControl == 0)
        MessageBox(NULL, L"Can not create combo bocx control", L"ComboControl", MB_OK | MB_ICONERROR);

    return hThisControl;
}

__int64 CComboControl::AddString(const wchar_t* text, __int64 itemData)
{
    if (text == 0)
        return 0;

    return (__int64)SendMessage(hThisControl, CB_ADDSTRING, (WPARAM)0, (LPARAM)text);
}

__int64 CComboControl::GetItemData(__int64 index)
{
    return (__int64)SendMessage(hThisControl, CB_GETITEMDATA, (WPARAM)index, (LPARAM)0);
}

__int64 CComboControl::GetCount()
{
    return (__int64)SendMessage(hThisControl, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
}

bool CComboControl::Select(__int64 index)
{
    return (SendMessage(hThisControl, CB_SETCURSEL, (WPARAM)index, (LPARAM)0) != CB_ERR) ? true : false;
}

__int64 CComboControl::GetSelected()
{
    return (__int64)SendMessage(hThisControl, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
}

CString CComboControl::GetItem(__int64 index)
{
    CString item;
    __int64 len = SendMessage(hThisControl, CB_GETLBTEXTLEN, (WPARAM)index, (LPARAM)0) + 1;
    wchar_t * buffer = new wchar_t[len] ;
    SendMessage(hThisControl, CB_GETLBTEXT, (WPARAM)index, (LPARAM)buffer);
    item = buffer;
    delete[]buffer;
    return item;
}
