#pragma once
#include "framework.h"

class CBaseMenu
{
public:
	CBaseMenu();
	CBaseMenu(HWND);
	~CBaseMenu();

	HMENU GetHMenu() { return hMenu; };

	bool Attach(HWND);

	HMENU CreateMenu();
	HMENU CreateSubMenu();
	bool  AddMenuItem(HMENU, const wchar_t*, int);
	bool  AddSubMenu(HMENU, const wchar_t*, HMENU);
	bool  AddSeparator(HMENU);
	void  SetMenuItemActive(HMENU, UINT, bool);
private:
	HWND hWnd;
	HMENU hMenu;
	HMENU* hSubMenu;
	size_t subNenuCount;

	bool addSubMenu(HMENU);
	bool deleteSubMenu(HMENU);
	void AdjustWindowHeight(bool hasMenu);
};

