#include "BaseProcessInfo.h"

typedef BOOL (WINAPI *EnumProcesses)(DWORD*, DWORD, DWORD*);
typedef DWORD (WINAPI* GetModuleBaseNameW)(HANDLE, HMODULE, LPWSTR, DWORD nSize);

CBaseProcessesInfo::CBaseProcessesInfo()
{}

CBaseProcessesInfo::~CBaseProcessesInfo()
{
	clear();
}

void CBaseProcessesInfo::Update()
{
	enum_processes();
	enum_windows();
}

void CBaseProcessesInfo::clear()
{
	if (info == 0)
		return;

	for (size_t i = 0; i < items_count; i++) {
		if (info[i].moduleBaseName != 0)
			delete[]info[i].moduleBaseName;
		if (info[i].windowCaption != 0)
			delete[]info[i].windowCaption;
	}
	delete[]info;
}


void CBaseProcessesInfo::enum_processes()
{
	HMODULE		hModule = LoadLibrary(L"psapi.dll");
	HANDLE		hProcess = 0;
	DWORD* process_id = 0;
	DWORD		items_count = 1000;
	DWORD		bytes_needed = 0;
	DWORD		factor = 1;
	DWORD       module_name_needed = 0;
	size_t      module_name_len = 0x10000;
	wchar_t* module_name = new wchar_t[module_name_len + 1];

	if (hModule == 0)
		return;

	EnumProcesses ptrEnumProcesses = (EnumProcesses)GetProcAddress(hModule, "EnumProcesses");
	GetModuleBaseNameW ptrGetModuleBaseName = (GetModuleBaseNameW)GetProcAddress(hModule, "GetModuleBaseNameW");

	if ((ptrEnumProcesses == 0) || (ptrGetModuleBaseName == 0)) {
		FreeModule(hModule);
		return;
	}

	process_id = new DWORD[items_count * sizeof(DWORD)];

	if (process_id == 0) {
		FreeModule(hModule);

		if (module_name != 0)
			delete[]module_name;

		return;
	}

	while (!ptrEnumProcesses(process_id, factor * items_count * sizeof(DWORD), &bytes_needed)) {
		if (bytes_needed == (factor * items_count * sizeof(DWORD))) {
			delete[]process_id;
			factor++;
			process_id = new DWORD[factor * items_count];

			if (process_id == 0) {
				FreeModule(hModule);

				if (module_name != 0)
					delete[]module_name;

				return;
			}
			continue;
		}
		break;
	}

	if (info != 0)
		delete[]info;

	info = new processInfo[bytes_needed / sizeof(DWORD)];

	if (info == 0) {
		FreeModule(hModule);
		delete[]process_id;

		if (module_name != 0)
			delete[]module_name;

		return;
	}

	items_count = bytes_needed / sizeof(DWORD);
	memset(info, 0, sizeof(processInfo) * items_count);

	for (size_t i = 0; i < items_count; i++) {
		info[i].dwProcessID = process_id[i];
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id[i]);

		if ((hProcess != 0) && (module_name != 0)) {

			wmemset(module_name, 0, module_name_len + 1);
			module_name_needed = ptrGetModuleBaseName(hProcess, 0, module_name, module_name_len);

			if (module_name_needed > 0) {
				info[i].moduleBaseName = new wchar_t[module_name_needed + 1];

				if (info[i].moduleBaseName != 0) {
					wmemset(info[i].moduleBaseName, 0, module_name_needed + 1);
					wmemcpy(info[i].moduleBaseName, module_name, module_name_needed);
				}
			}
			CloseHandle(hProcess);
		}
	}
	CBaseProcessesInfo::items_count = items_count;
	FreeModule(hModule);
	delete[]process_id;
	delete[]module_name;
}

void CBaseProcessesInfo::enum_windows()
{
	HWND hWnd = 0;
	EnumWindows(EnumWindowsProc, (LPARAM)this);
}

BOOL CALLBACK CBaseProcessesInfo::EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
	CBaseProcessesInfo* ptrObject		= (CBaseProcessesInfo*)lParam;
	DWORD				windowProcessId = 0;
	DWORD				windowThreadId	= GetWindowThreadProcessId(hWnd, &windowProcessId);
	size_t				buffer_len		= 1024;
	size_t				actual_len		= 0;
	wchar_t*			buffer			= new wchar_t[buffer_len + 1];

	for (size_t i = 0; i < ptrObject->items_count; i++) {
		if (windowProcessId == ptrObject->info[i].dwProcessID) {
			if (IsWindowVisible(hWnd)){
				ptrObject->info[i].hWnd = hWnd;
				if (buffer != 0) {
					wmemset(buffer, 0, buffer_len);
					actual_len = GetWindowText(hWnd, buffer, buffer_len);
					if (actual_len > 0) {
						ptrObject->info[i].windowCaption = new wchar_t[actual_len + 1];
						if (ptrObject->info[i].windowCaption != 0) {
							wmemset(ptrObject->info[i].windowCaption, 0, actual_len + 1);
							wmemcpy(ptrObject->info[i].windowCaption, buffer, actual_len);
						}
					}
				}
			}
			if (buffer != 0)
				delete[]buffer;
			return true;
		}
	}
	
	if (buffer != 0)
		delete[]buffer;

	return true;
}


const processInfo* CBaseProcessesInfo::GetItem(size_t index)
{
	if (index >= items_count)
		return (processInfo*)0;

	return &info[index];
}