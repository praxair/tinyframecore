#pragma once
#include "BaseWnd.h"
#include "EditControl.h"
#include "LabelControl.h"
#include "ButtonControl.h"

class CBaseInputBox : public CBaseWnd
{
public:
	CBaseInputBox(HWND hParent);
	~CBaseInputBox();

	const wchar_t* Message(const wchar_t* promt, const wchar_t* caption, const wchar_t* default_value);

private:
	HWND Create();
	bool OnCommand(unsigned __int16 commandId /*LOWORD(wParam)*/, unsigned __int16 msg /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/);

	void create_label();
	void create_editbox();
	void create_buton_ok();
	void create_buton_cancel();
	void save_value();
	void erase_value();

	bool OnCreate(CREATESTRUCT* lParam);
	bool OnDestroy();

	CLabelControl*  label_question;
	CEditControl*   editbox_value;
	CButtonControl* button_ok;
	CButtonControl* button_cancel;

	wchar_t* value;
	HWND parent;
};

