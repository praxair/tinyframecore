#include "MultilineEditControl.h"

CMultilineEditControl::CMultilineEditControl()
{
}

CMultilineEditControl::~CMultilineEditControl()
{
	if (text != 0)
		delete[]text;
}

HWND CMultilineEditControl::Create(HWND hParent, int controlId, RECT& rect)
{
	hThisControl = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_VISIBLE | WS_CHILD |  ES_LEFT | ES_AUTOHSCROLL | ES_MULTILINE | ES_WANTRETURN | ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL, rect.left, rect.top, rect.right, rect.bottom, hParent, (HMENU)controlId, GetModuleHandle(0), NULL);

	HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
	SendMessage(hThisControl, WM_SETFONT, (WPARAM)hFont, TRUE);
	
	return hThisControl;
}


void CMultilineEditControl::SetText(wchar_t* new_text)
{
	size_t length = 0;

	if (text != 0) {
		delete[]text;
		SendMessage(hThisControl, WM_SETTEXT, (WPARAM)0, (LPARAM)text);
	}

	if (new_text == 0)
		return;

	length = wcslen(new_text);

	text = new wchar_t[length + 1];

	if (text == 0)
		return;

	wmemset(text, 0, length + 1);
	wmemcpy(text, new_text, length);

	SendMessage(hThisControl, WM_SETTEXT, (WPARAM)0, (LPARAM)text);
}
