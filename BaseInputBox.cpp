#include "BaseInputBox.h"

CBaseInputBox::CBaseInputBox(HWND parent)
{
	this->parent = parent;
}


CBaseInputBox::~CBaseInputBox()
{
	erase_value();
}


HWND CBaseInputBox::Create()
{
	const wchar_t* class_name = L"BaseModalWindow";
	if (!CBaseWnd::Register(class_name))
		return 0;

	hThisWindow = CreateWindowEx( WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE, class_name, L"Input value...", DS_MODALFRAME & 0xFFFF0000 & ~WS_VISIBLE, 0, 0, 500, 140, parent, 0, GetModuleHandle(0), (LPVOID)this);
	
	return hThisWindow;
}


bool CBaseInputBox::OnCreate(CREATESTRUCT* lParam)
{
	Center(parent);
	SetForegroundWindow(hThisWindow);
	EnableWindow(parent, FALSE);

	create_label();
	create_editbox();
	create_buton_ok();
	create_buton_cancel();

	ShowWindow(hThisWindow, SW_SHOW);
	UpdateWindow(hThisWindow);

	return true;
}


bool CBaseInputBox::OnDestroy()
{
	EnableWindow(parent, TRUE);
	SetForegroundWindow(parent);
	SendMessage(hThisWindow, WM_CLOSE, 0, 0);

	return true;
}


bool CBaseInputBox::OnCommand(unsigned __int16 commandId /*LOWORD(wParam)*/, unsigned __int16 msg /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/)
{
	switch (commandId)
	{
	case 993/*OK*/:
		save_value();
		EnableWindow(parent, TRUE);
		SetForegroundWindow(parent);
		SendMessage(hThisWindow, WM_CLOSE, 0, 0);
		break;
	case 994 /*Cancel*/:
		erase_value();
		EnableWindow(parent, TRUE);
		SetForegroundWindow(parent);
		SendMessage(hThisWindow, WM_CLOSE, 0, 0);
		break;
	}

	return true;
}


void CBaseInputBox::create_label()
{
	RECT rect = { 0 };
	label_question = new CLabelControl;
	
	if (label_question == 0)
		return;

	label_question->Create(hThisWindow, 991, rect = { 10, 10, 150, 25 });
	label_question->SetText(L"Input a value...");
	label_question->Show();
}


void CBaseInputBox::create_editbox()
{
	RECT rect = { 0 };
	editbox_value = new CEditControl;
	
	if (editbox_value == 0)
		return;

	editbox_value->Create(hThisWindow, 992, rect = { 170, 10, 300, 25 });
	editbox_value->SetText(L"");
	
	SendMessage(editbox_value->GetHwnd(), EM_SETSEL, 0, -1);
	SetFocus(editbox_value->GetHwnd());
	editbox_value->Show();
}


void CBaseInputBox::create_buton_ok()
{
	RECT rect = { 0 };
	button_ok = new CButtonControl;
	
	if (button_ok == 0)
		return;

	button_ok->Create(hThisWindow, 993, rect = { 370, 60, 100, 30 });
	button_ok->SetText(L"OK");
	button_ok->Show();
}


void CBaseInputBox::create_buton_cancel()
{
	RECT rect = { 0 };
	button_cancel = new CButtonControl;
	
	if (button_cancel == 0)
		return;

	button_cancel->Create(hThisWindow, 994, rect = { 240, 60, 100, 30 });
	button_cancel->SetText(L"Cancel");
	button_cancel->Show();
}

const wchar_t* CBaseInputBox::Message(const wchar_t* promt, const wchar_t* caption, const wchar_t* default_value)
{
	MSG msg = { 0 };
	HWND current_window = 0;

	// create the InputBox main window
	Create();

	// Set InputBox main window's caption
	if (caption != 0)
		SetCaption(caption);

	// Create all its controls
	if ((label_question != 0) && (promt != 0))
		label_question->SetText(promt);

	if ((editbox_value != 0) && (default_value != 0)) {
		editbox_value->SetText(default_value);
		editbox_value->SelText(0, -1);
		SetFocus(editbox_value->GetHwnd());
	}

	// Get into eternal message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (msg.message == WM_KEYDOWN)
		{
			if (msg.wParam == VK_ESCAPE) 
			{
				erase_value();
				SendMessage(hThisWindow, WM_DESTROY, 0, 0);
				//return 0;
			}
			if (msg.wParam == VK_RETURN) 
			{
				save_value();
				SendMessage(hThisWindow, WM_DESTROY, 0, 0);
				//return value;
			}
			if (msg.wParam == VK_TAB)
			{
				current_window = GetFocus();
				if (current_window == editbox_value->GetHwnd()) SetFocus(button_ok->GetHwnd());
				if (current_window == button_ok->GetHwnd()) SetFocus(button_cancel->GetHwnd());
				if (current_window == button_cancel->GetHwnd()) SetFocus(editbox_value->GetHwnd());
			}

		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return value;
}


void CBaseInputBox::save_value()
{
	size_t len = editbox_value->GetLen();

	if (value != 0) {
		delete[]value;
		value = 0;
	}

	value = new wchar_t[len + 2];

	if (value == 0)
		return;

	wmemset(value, 0, len + 2);
	GetWindowText(editbox_value->GetHwnd(), value, len + 1);
}


void CBaseInputBox::erase_value()
{
	if (value == 0)
		return;

	delete[]value;
	value = 0;
}