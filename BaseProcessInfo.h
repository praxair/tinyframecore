#pragma once
#include "framework.h"

typedef struct processInfo
{
	DWORD dwProcessID;
	HWND hWnd;
	wchar_t* moduleBaseName;
	wchar_t* windowCaption;
}processInfo;

class CBaseProcessesInfo
{
public:
	CBaseProcessesInfo();
	~CBaseProcessesInfo();

	void Update();
	const processInfo* GetItem(size_t);
	size_t GetItemsCount() {return items_count;};
private:
	void clear();
	void enum_windows();
	void enum_processes();

	static BOOL CALLBACK EnumWindowsProc(HWND   hwnd, LPARAM lParam);

	processInfo* info;
	size_t items_count;
};

