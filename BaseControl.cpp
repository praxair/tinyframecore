#include "BaseControl.h"

CBaseControl::CBaseControl()
{
    hParentWindow = 0;
    hThisControl = 0;
    controlId = 0;
    caption = 0;
}


CBaseControl::~CBaseControl()
{
    if (caption != 0)
        delete[]caption;
}


void CBaseControl::Show()
{
    ShowWindow(hThisControl, SW_SHOW);
}


void CBaseControl::Hide()
{
    ShowWindow(hThisControl, SW_HIDE);
}


void CBaseControl::Size(int x, int y, int width, int height)
{
    MoveWindow(hThisControl, x, y, width, height, true);
}

void CBaseControl::SetCaption(const wchar_t* caption)
{
    size_t newLen = 0;

    if (caption == 0)
        return;

    if (CBaseControl::caption != 0)
    {
        delete[]CBaseControl::caption;
        CBaseControl::caption = 0;
    }

    newLen = wcsnlen_s(caption, (SIZE_MAX - 1));
    CBaseControl::caption = new wchar_t[newLen + 1];
    if (CBaseControl::caption == 0)
        return;

    wmemset(CBaseControl::caption, 0, newLen + 1);
    wmemcpy(CBaseControl::caption, caption, newLen);

    //SetWindowText(hThisControl, CBaseControl::caption);
}