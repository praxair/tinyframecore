#include "ButtonControl.h"


CButtonControl::CButtonControl()
{}

CButtonControl::~CButtonControl()
{
	if (text != 0)
		delete[]text;
}


HWND CButtonControl::Create(HWND hParent, int controlId, RECT& rect)
{
	hThisControl = CreateWindowEx(WS_EX_CLIENTEDGE, L"BUTTON", L"...", WS_VISIBLE | WS_CHILD , rect.left, rect.top, rect.right, rect.bottom, hParent, (HMENU)controlId, GetModuleHandle(0), NULL);

	return hThisControl;
}

void CButtonControl::SetText(const wchar_t* new_text)
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

