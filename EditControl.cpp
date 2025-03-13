#include "EditControl.h"

CEditControl::CEditControl()
{}


CEditControl::~CEditControl()
{
}

HWND CEditControl::Create(HWND hParent, int controlId, RECT& rect)
{

	hThisControl = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL | ES_NOHIDESEL, rect.left, rect.top, rect.right, rect.bottom, hParent, (HMENU)controlId, GetModuleHandle(0), NULL);
	
	HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

	SendMessage(hThisControl, WM_SETFONT, (WPARAM)hFont, TRUE);

	return hThisControl;
}

void CEditControl::SetText(const wchar_t* text)
{
	SendMessage(hThisControl, WM_SETTEXT, (WPARAM)0, (LPARAM)text);
}

void CEditControl::SelText(int start, int length)
{
	SendMessage(hThisControl, EM_SETSEL, start, length);
}

size_t CEditControl::GetLen()
{
	return GetWindowTextLength(hThisControl);
}