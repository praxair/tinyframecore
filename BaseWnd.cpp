#include "BaseWnd.h"
#include "Windowsx.h"
#include "commctrl.h"
#include "dwmapi.h"
#include "shobjidl_core.h"

// prototype for dynamic loading "InitCommonControlsEx()" from "Comctl32.dll"
// you can do it explicitly by adding .lib file into field "Additional Dependencies"
// flollows
// "Configuration Properties" -> "Linker" -> "Input"
// or
// "Проект" -> "Свойства" -> "Компоновщик" -> "Общие"
// добавить в конец списка "Выходной файл" через точку с запятой
typedef int (WINAPI* ptrInitCommonControlsEx)(INITCOMMONCONTROLSEX*);
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

CBaseWnd::CBaseWnd()
{
	//AdjustWindowRect and AdjustWindowRectEx?
	hMenu = 0;
	timer = 0;
	timerId = 1;
	timerInterval = 0;
	hThisWindow = 0;
	hParentWindow = 0;
	defaultWidth = 400;
	defaultHeight = 200;
	windowCaption = L"Base Window";
	className = 0;
	
	// Notification area icon
	memset(&nid, 0, sizeof(NOTIFYICONDATA));
	nid.cbSize = sizeof(nid);
}

CBaseWnd::~CBaseWnd()
{
	if (timer != 0)
		KillTimer(hThisWindow, timerId);

	if (hThisWindow)
		DestroyWindow(hThisWindow);
	
	if (className != 0)
		delete[]className;

	return;
}


void CBaseWnd::IniCommontControls(DWORD controlClass)
{
	INITCOMMONCONTROLSEX icx = { 0 };
	HMODULE hLibrary = LoadLibrary(L"Comctl32.dll");
	ptrInitCommonControlsEx _InitCommonControlsEx = 0;

	if (hLibrary == 0)
		return;

	_InitCommonControlsEx = (ptrInitCommonControlsEx)GetProcAddress(hLibrary, "InitCommonControlsEx");

	if (_InitCommonControlsEx == 0) {
		FreeLibrary(hLibrary);
		return;
	}

	// Initialize Common Controls to get use TreeView and ListView 
	icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icx.dwICC = controlClass;
	_InitCommonControlsEx(&icx);
	FreeLibrary(hLibrary);
}

HWND CBaseWnd::Create(HWND hParent = 0, const wchar_t* caption = 0)
{
	if (!Register(className))
		return (HWND)0;

	IniCommontControls(ICC_TREEVIEW_CLASSES| ICC_USEREX_CLASSES| ICC_STANDARD_CLASSES| ICC_LISTVIEW_CLASSES);

	if (caption != 0)
		windowCaption = caption;

	hParentWindow = hParent;

	hThisWindow = CreateWindowEx(NULL,
								 className,
								 windowCaption,
								 WS_OVERLAPPEDWINDOW,
								 CW_USEDEFAULT,
								 CW_USEDEFAULT,
								 defaultWidth,
								 defaultHeight,
								 hParent,
								 hMenu,
								 GetModuleHandle(NULL),
								 (LPVOID)this);

	if (hThisWindow == 0)
	{
		wchar_t buffer[250] = { 0 };
		wsprintf(buffer, L"Error: 0x%X", GetLastError());
		MessageBox(NULL, buffer, L"Base Window Class", MB_OK);
	}

	return hThisWindow;
}

void CBaseWnd::Show()
{
	if (hThisWindow != 0)
		ShowWindow(hThisWindow, SW_SHOW);
}

void CBaseWnd::Hide()
{
	if (hThisWindow != 0)
		ShowWindow(hThisWindow, SW_HIDE);
}

void CBaseWnd::Minimize()
{
	if (hThisWindow != 0)
		ShowWindow(hThisWindow, SW_MINIMIZE);
}


void CBaseWnd::Restore()
{
	if (hThisWindow != 0)
		ShowWindow(hThisWindow, SW_RESTORE);
}


bool CBaseWnd::SetCaption(const wchar_t* caption)
{
	return SetWindowText(hThisWindow, caption);
}

// Returns:
//	true	- If window's class is registered within a system
//	false	- Window's classs has not been registerd. Any error occured
bool CBaseWnd::Register(const wchar_t* class_name)
{
	size_t len = 0;
	const wchar_t* default_class_name = L"BaseWindowClass";

	WNDCLASSEX wcx = { 0 };

	if (class_name != 0) {
		len = wcslen(class_name);
		className = new wchar_t[len + 1];
		
		if (className == 0)
			return false;

		wmemset(className, 0, len + 1);
		wmemcpy(className, class_name, len);
	}
	else
	{
		len = wcslen(default_class_name);
		className = new wchar_t[len + 1];

		wmemset(className, 0, len + 1);
		wmemcpy(className, default_class_name, len);
	}

	// Check if window's class is already registered
	if (GetClassInfoEx(GetModuleHandle(NULL), class_name, &wcx))
		return true;

	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hIcon = NULL;
	wcx.hIconSm = NULL;
	wcx.lpszMenuName = NULL;
	wcx.lpfnWndProc = BaseWndProc;
	wcx.lpszClassName = className;
	wcx.style = CS_HREDRAW | CS_VREDRAW;
	wcx.hInstance = GetModuleHandle(NULL);
	wcx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcx.hCursor = LoadCursor(GetModuleHandle(NULL), IDC_ARROW);

	return RegisterClassEx(&wcx);
}

void CBaseWnd::Center(HWND hWnd)
{
	RECT refRect  = { 0 };
	RECT thisRect = { 0 };
	RECT tmpRect  = { 0 };
	HWND hReferenceWnd = hWnd;
	
	if (hReferenceWnd == NULL)
		hReferenceWnd = GetDesktopWindow();

	GetWindowRect(hThisWindow, &thisRect);
	GetWindowRect(hReferenceWnd, &refRect);
	CopyRect(&tmpRect, &refRect);

	OffsetRect(&thisRect, -thisRect.left, -thisRect.top);
	OffsetRect(&tmpRect, -tmpRect.left, -tmpRect.top);
	OffsetRect(&tmpRect, -thisRect.right, -thisRect.bottom);

	SetWindowPos(hThisWindow, HWND_TOP, refRect.left + (tmpRect.right / 2), refRect.top + (tmpRect.bottom / 2), 0, 0, SWP_NOSIZE);
}

void CBaseWnd::Size(int width, int height)
{
	RECT rect = { 0 };
	
	GetWindowRect(hThisWindow, &rect);
	MoveWindow(hThisWindow, rect.left, rect.top, width, height, true);
}

void CBaseWnd::SetTimer(UINT interval)
{
	timerInterval = interval;

	// First kill timer
	if (timer != 0)
	{
		if (KillTimer(hThisWindow, timerId))
			timer = 0;
		else
			return;
	}

	// Check if interval is greater than 0
	if (timerInterval == 0)
		return;

	// Create new timer
	::SetTimer(hThisWindow, timerId, timerInterval, 0);
}


// MAKEINTRESOURCE
// pass 0 to remove overlay
bool CBaseWnd::SetOverlayIcon(int id)
{
	ITaskbarList3 * tbl = 0;
	THUMBBUTTON thumbButtons[1];
	HRESULT hResult = 0;
	HICON icon = 0;

	CoInitialize(NULL);

	hResult = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&tbl));
	
	if (hResult != S_OK)
		return false;

	if (tbl->HrInit() != S_OK) {
		tbl->Release();
		return false;
	}
	ZeroMemory(thumbButtons, sizeof(THUMBBUTTON));
	icon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(id));
	thumbButtons[0].dwMask = THB_ICON;
	//thumbButtons[0].iId = 1;// Here must be our button identifier
	thumbButtons[0].hIcon = icon;
	
	//tbl->ThumbBarUpdateButtons(hThisWindow, 0, thumbButtons);
	tbl->SetOverlayIcon(hThisWindow, icon, L"DSd");

	DeleteObject((HGDIOBJ)icon);
	tbl->Release();

	return true;
}


// MAKEINTRESOURCE
bool CBaseWnd::SetIcon(int id)
{
	HICON hIcon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(id));

	if (hIcon == 0)
		return false;

	SendMessage(hThisWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hThisWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	return true;
}


// LoadImage
// From File
bool CBaseWnd::SetIcon(const wchar_t* path, size_t width, size_t height)
{
	HICON hIcon = (HICON)LoadImage(0, path, IMAGE_ICON, width, height, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

	if (hIcon == 0)
		return false;

	SendMessage(hThisWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hThisWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	return true;
}


// LoadImage
// From Resource
bool CBaseWnd::SetIcon(int id, size_t width, size_t height)
{
	HICON hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(id), IMAGE_ICON, width, height, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT | LR_SHARED);

	if (hIcon == 0)
		return false;

	SendMessage(hThisWindow, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
	SendMessage(hThisWindow, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

	return true;
}


bool CBaseWnd::SetNotifyIcon(int id, size_t width, size_t height, const wchar_t* caption)
{
	size_t length = 0;
	
	nid.hWnd = hThisWindow;
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID ;
	nid.uID = 0x7FFF;
	// Uncomment lines below to get notification balloons
	//nid.uFlags |= NIF_INFO;
	//nid.uTimeout = 10;
	//nid.dwInfoFlags |= NIIF_INFO | NIIF_NOSOUND | NIIF_LARGE_ICON;
	//wmemcpy(nid.szInfoTitle, L"capta", 5);
	//wmemcpy(nid.szInfo, L"recording...", 12);

	if (caption != 0) {
		length = wcslen(caption);
		wmemset(nid.szTip, 0, length + 1);
		wmemcpy(nid.szTip, caption, length);
	}

	nid.hIcon = (HICON)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(id), IMAGE_ICON, width, height, LR_DEFAULTCOLOR | LR_LOADTRANSPARENT);

	if (!Shell_NotifyIcon(NIM_ADD, &nid))
		return false;
	
	return Shell_NotifyIcon(NIM_MODIFY, &nid);
}

bool CBaseWnd::DeleteNotifyIcon()
{
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}

bool CBaseWnd::OnCommand(unsigned __int16 id /*LOWORD(wParam)*/, unsigned __int16 msg /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/)
{
	return true;// false;
}


bool CBaseWnd::OnCreate(CREATESTRUCT* cs)
{
	return false;
}


bool CBaseWnd::OnClose()
{
	DestroyWindow(hThisWindow);
	PostQuitMessage(0);
	return true;
}

bool CBaseWnd::OnDestroy()
{
	return false;
}

bool CBaseWnd::OnKeyDown(unsigned __int64 vKeyCode, bool keyAltDown, bool keyShiftDown, bool keyCtrlDown)
{
	return false;
}


bool CBaseWnd::OnKeyUp(unsigned __int64 vKeyCode, bool keyAltDown, bool keyShiftDown, bool keyCtrlDown)
{
	return false;
}

bool CBaseWnd::OnNotify(unsigned __int16 commandId /*LOWORD(wParam)*/, unsigned __int16 parameter /*HIWORD(wParam)*/, __int64 hControlWnd /*lParam*/, unsigned __int16 x /*GET_X_LPARAM(wParam)*/, unsigned __int16 y /*GET_Y_LPARAM(wParam)*/)
{
	return false;
}

bool CBaseWnd::OnPaint(PAINTSTRUCT* ps)
{
	return false;
}

bool CBaseWnd::OnSize()
{
	return false;
}

bool CBaseWnd::OnShow()
{
	return false;
}

bool CBaseWnd::OnSysCommand(unsigned __int64 commandId /*wParam*/, unsigned __int16 x /*LOWORD(wParam)*/, unsigned __int16 y /*HIWORD(wParam)*/)
{
	return false;
}

// allways return true
bool CBaseWnd::OnTimer(UINT_PTR id)
{
	RECT rect = { 0 };

	GetWindowRect(hThisWindow, &rect);
	Size(rect.right - rect.left + 1, rect.bottom - rect.top + 1);

	return true;
}

// The single message processing routine for all windows of inherited classes
LRESULT CALLBACK CBaseWnd::BaseWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CREATESTRUCT* cs = 0;
	CBaseWnd * ptrThis = 0;

	if (msg == WM_NCCREATE) {
		cs = (CREATESTRUCT*)lParam;
		ptrThis = (CBaseWnd*)(cs->lpCreateParams);
		ptrThis->hThisWindow = hWnd;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)ptrThis);
	}
	else
	{
		ptrThis = (CBaseWnd*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	if (!ptrThis)
		return DefWindowProc(hWnd, msg, wParam, lParam);
		
	if (!(ptrThis->WndProc(msg, wParam, lParam)))
		return DefWindowProc(hWnd, msg, wParam, lParam);

	return 0;
}

// This function binds window's messages to appropriate methods
// Returns true in message has been processed. Otherwise returns false
bool CBaseWnd::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		return OnClose();
	case WM_COMMAND:
		return OnCommand(LOWORD(wParam) /*commandId*/, HIWORD(wParam) /*msg*/, lParam /*hControlWnd*/);
	case WM_CREATE:
		return OnCreate((CREATESTRUCT*)lParam);
	case WM_DESTROY:
		return OnDestroy();
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		/* LOWORD(wParam)         == Virtual KeyCode
		* HIWORD(lParam)          == Flags
		* LOBYTE(HIWORD(lParam))  == OEM scan code */
		return OnKeyDown(LOWORD(wParam)									/*Virtual Key Code*/,
					GetKeyState(VK_MENU) < 0 ? true : false,		/* Alt key down */
					GetKeyState(VK_SHIFT) < 0 ? true : false,		/* Shift key down */
					GetKeyState(VK_CONTROL) < 0 ? true : false);	/* Control key down */
	case WM_KEYUP:
	case WM_SYSKEYUP:
		/* LOWORD(wParam)         == Virtual KeyCode
		* HIWORD(lParam)          == Flags
		* LOBYTE(HIWORD(lParam))  == OEM scan code */
		return OnKeyUp(LOWORD(wParam)									/*Virtual Key Code*/,
					GetKeyState(VK_MENU) < 0 ? true : false,		/* Alt key down */
					GetKeyState(VK_SHIFT) < 0 ? true : false,		/* Shift key down */
					GetKeyState(VK_CONTROL) < 0 ? true : false);	/* Control key down */
	case WM_NOTIFY:
		return OnNotify(LOWORD(wParam) /*commandId*/, HIWORD(wParam) /*parameter*/, lParam /*hControlWnd*/, GET_X_LPARAM(lParam) /*x*/, GET_Y_LPARAM(lParam) /*y*/);
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hThisWindow, &ps);
		bool fReturn = OnPaint(&ps);
		EndPaint(hThisWindow, &ps);
		return fReturn;
	}
	case WM_SHOWWINDOW:
		return OnShow();
	case WM_SIZE:
		return OnSize();
	case WM_SYSCOMMAND:
		return OnSysCommand(wParam /*commandId*/, GET_X_LPARAM(lParam) /*x*/, GET_Y_LPARAM(lParam) /*y*/);
	case WM_TIMER:
		return OnTimer(wParam);
	}
	
	// the message has no appropriate handler
	return false;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	wchar_t caption[255] = { 0 };
	wchar_t class_name[255] = { 0 };

	size_t len = GetWindowTextLength(hwnd);
	if (len > 0)
		GetWindowText(hwnd, caption, 255);
	
	GetClassName(hwnd, class_name, 255);
	//"ToolbarWindow32" L"ToolbarWindow32"	wchar_t[0x000000ff]

	return TRUE;
}

bool CBaseWnd::SetMenu(HMENU hNewMenu)
{
	RECT rect_orig = { 0 };
	RECT rect = { 0 };

	if (hNewMenu == 0)
		return false;

	hMenu = hNewMenu;
	
	if (!(::SetMenu(hThisWindow, hNewMenu)))
		return false;
	
	if (!DrawMenuBar(hThisWindow))
		return false;
	
	if (!GetWindowRect(hThisWindow, &rect))
		return false;

	// save original rect for future calculation
	rect_orig = rect;

	if (!AdjustWindowRectEx(&rect, GetWindowLong(hThisWindow, GWL_STYLE), TRUE, GetWindowLong(hThisWindow, GWL_EXSTYLE)))
		return false;

	return MoveWindow(hThisWindow, rect_orig.left, rect_orig.top, rect.right - rect_orig.left, rect.bottom - rect_orig.top, TRUE);
}