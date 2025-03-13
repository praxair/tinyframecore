#include "BaseMenu.h"

CBaseMenu::CBaseMenu()
{
	hWnd = 0;
	hSubMenu = 0;
	subNenuCount = 0;
	hMenu = CreateMenu();
}


CBaseMenu::CBaseMenu(HWND hwnd)
{
	hWnd = hwnd;
	hSubMenu = 0;
	subNenuCount = 0;
	hMenu = CreateMenu();
	
	if (hwnd != 0) {
		SetMenu(hwnd, hMenu);
		AdjustWindowHeight(true);
	}
}


CBaseMenu::~CBaseMenu()
{
	if (hMenu != 0)
		DestroyMenu(hMenu);

	if (hSubMenu != 0)
		delete[]hSubMenu;
	
	AdjustWindowHeight(false);
}


bool CBaseMenu::Attach(HWND hwnd)
{
	if ((hMenu == 0) || (hwnd == 0))
		return false;
	
	hWnd = hwnd;

	if (!SetMenu(hwnd, hMenu))
		return false;

	AdjustWindowHeight(true);

	return DrawMenuBar(hWnd);
}

HMENU CBaseMenu::CreateMenu()
{
	return ::CreateMenu();
}


HMENU CBaseMenu::CreateSubMenu()
{
	HMENU hmenu = CreatePopupMenu();

	if (hmenu != 0)
		addSubMenu(hmenu);

	return hmenu;
}

bool CBaseMenu::AddMenuItem(HMENU hmenu, const wchar_t* caption, int command_id)
{
	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_STRING| MIIM_ID;
	mii.fType = MFT_STRING;
	mii.fState = MFS_ENABLED;
	mii.wID = command_id;
	mii.dwTypeData = (LPTSTR)caption;
	mii.cch = wcslen(caption);

	if (hmenu == 0) {
		if (InsertMenuItem(hMenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}
	else {
		if (InsertMenuItem(hmenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}
	return false;
}

bool CBaseMenu::AddSeparator(HMENU hmenu)
{
	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_FTYPE;
	mii.fType = MFT_SEPARATOR;

	if (hmenu == 0) {
		if (InsertMenuItem(hMenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}
	else {
		if (InsertMenuItem(hmenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}

	return false;

}

bool CBaseMenu::AddSubMenu(HMENU hmenu, const wchar_t* caption, HMENU hSubMenu)
{
	MENUITEMINFO mii = { 0 };
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_SUBMENU | MIIM_STRING;
	mii.fType = MFT_STRING;
	mii.fState = MFS_ENABLED;
	mii.wID = (UINT)hSubMenu;
	mii.dwTypeData = (LPTSTR)caption;
	mii.cch = wcslen(caption);
	mii.hSubMenu = hSubMenu;

	if (hmenu == 0) {
		if (InsertMenuItem(hMenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}
	else {
		if (InsertMenuItem(hmenu, GetMenuItemCount(hmenu), true, &mii))
			return DrawMenuBar(hWnd);
	}

	return false;
}


bool CBaseMenu::addSubMenu(HMENU new_item)
{
	HMENU* tmp_array = new HMENU[subNenuCount + 1];

	if (tmp_array == 0)
		return false;

	if (hSubMenu == 0) {
		hSubMenu = tmp_array;
		return true;
	}

	memcpy(tmp_array, hSubMenu, subNenuCount * sizeof(HMENU));
	tmp_array[subNenuCount++] = new_item;

	delete[] hSubMenu;
	
	hSubMenu = tmp_array;
	
	return true;
}


bool CBaseMenu::deleteSubMenu(HMENU item)
{
	HMENU* tmp_array = 0;

	if (hSubMenu == 0)
		return false;

	for (size_t i = 0; i < subNenuCount; i++) {
		if (item == hSubMenu[i]) {
			if (subNenuCount == 1) {
				delete[] hSubMenu;
				subNenuCount = 0;
				return true;
			}
			
			tmp_array = new HMENU[subNenuCount - 1];
			
			if (tmp_array == 0)
				return false;

			memcpy(tmp_array, hSubMenu, i * sizeof(HMENU));

			if ( i < (subNenuCount - 1))
				memcpy(&tmp_array[i], &hSubMenu[i+1], (subNenuCount - i - 1) * sizeof(HMENU));

			delete[] hSubMenu;
			hSubMenu = tmp_array;
			subNenuCount--;
			return true;
		}
	}
	return false;
}

void CBaseMenu::AdjustWindowHeight(bool hasMenu)
{
	RECT rect_wnd = { 0 };
	RECT rect_scr = { 0 };
	DWORD styles = GetWindowLong(hWnd, GWL_STYLE);
	DWORD xstyles = GetWindowLong(hWnd, GWL_EXSTYLE);
	GetWindowRect(hWnd, &rect_scr);
	GetClientRect(hWnd, &rect_wnd);
	if (AdjustWindowRectEx(&rect_wnd, styles, hasMenu, xstyles))
		MoveWindow(hWnd, rect_scr.left, rect_scr.top, rect_wnd.right - rect_wnd.left, rect_wnd.bottom - rect_wnd.top, TRUE);
}

void CBaseMenu::SetMenuItemActive(HMENU hmenu, UINT id, bool active)
{
	EnableMenuItem(hmenu, id, MF_BYCOMMAND | ( active ? MF_ENABLED : MF_GRAYED));
}