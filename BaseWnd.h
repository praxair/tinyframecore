#pragma once
#include "framework.h"
#include "Shellapi.h"

class CBaseWnd
{
public:
	CBaseWnd();
	~CBaseWnd();

	virtual HWND Create(HWND hParent, const wchar_t* caption);
	virtual void Show();
	virtual void Hide();
	virtual void Minimize();
	virtual void Restore();
	virtual void Center(HWND hWnd);
	virtual void Size(int width, int height);
	virtual bool SetCaption(const wchar_t* caption);
	virtual bool SetIcon(int id); // MAKEINTRESOURCE
	virtual bool SetIcon(const wchar_t*, size_t height, size_t width); // From File
	virtual bool SetIcon(int, size_t height, size_t width); //From Resource
	virtual bool SetMenu(HMENU);
	virtual bool SetNotifyIcon(int, size_t height, size_t width, const wchar_t*); //From Resource
	virtual bool SetOverlayIcon(int id); // MAKEINTRESOURCE
	virtual void SetTimer(UINT interval);

	virtual bool DeleteNotifyIcon();
	
	virtual HWND GetHwnd() { return hThisWindow; };
	virtual HWND GetParentHwnd() { return hParentWindow; };
	virtual bool IsVisible() { return IsWindowVisible(hThisWindow); }

	virtual bool OnCommand(unsigned __int16 commandId /*LOWORD(wParam)*/, unsigned __int16 msg /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/);
	virtual bool OnCreate(CREATESTRUCT* cs);
	virtual bool OnClose();
	virtual bool OnDestroy();
	virtual bool OnKeyDown(unsigned __int64 vKeyCode, bool keyAltDown, bool keyShiftDown, bool keyCtrlDown);
	virtual bool OnKeyUp(unsigned __int64 vKeyCode, bool keyAltDown, bool keyShiftDown, bool keyCtrlDown);
	virtual bool OnNotify(unsigned __int16 commandId /*LOWORD(wParam)*/, unsigned __int16 parameter /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/, unsigned __int16 x /*GET_X_LPARAM(wParam)*/, unsigned __int16 y /*GET_Y_LPARAM(wParam)*/);
	virtual bool OnPaint(PAINTSTRUCT* ps);
	virtual bool OnSize();
	virtual bool OnShow();
	virtual bool OnSysCommand(unsigned __int64 commandId /*wParam*/, unsigned __int16 x /*GET_X_LPARAM(wParam)*/, unsigned __int16 y /*GET_Y_LPARAM(wParam)*/ );
	virtual bool OnTimer(UINT_PTR id);

private:
	// The single message processing routine for all windows of inherited classes
	static LRESULT CALLBACK BaseWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HMENU hMenu;

	size_t defaultWidth;
	size_t defaultHeight;

	wchar_t* className;

	UINT_PTR timer;
	UINT_PTR timerId;
	UINT timerInterval;
	NOTIFYICONDATA nid;
protected:
	HWND hThisWindow;
	HWND hParentWindow;

	// Initialize common controls
	void IniCommontControls(DWORD);

	// Register Window Class
	bool Register(const wchar_t*);

	// if true returned then a message has been processed. false to call DefWindowProc
	virtual bool WndProc(UINT msg, WPARAM wParam, LPARAM lParam);
	const wchar_t* windowCaption;
};

