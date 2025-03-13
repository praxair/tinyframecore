#include "BaseLogger.h"

CBaseLogger::CBaseLogger()
{
	init();
}

CBaseLogger::CBaseLogger(wchar_t* path)
{
	init();
	this->path = path;

	open();
}

CBaseLogger::CBaseLogger(CString path)
{
	init();
	this->path = path;
	
	open();
}


CBaseLogger::~CBaseLogger()
{
	// we do not need to call CloseHandle in case of getting console handle with GetStdHandle
	FreeConsole();
	close();
}


void CBaseLogger::UseConsole(bool use)
{
	use_console = use;
}


bool CBaseLogger::Clear()
{
	int err = 0; // use int instead of weird errno_t which is just simple int itself
	
	close();

	err = _wfopen_s(&file, path.GetBuffer(), L"w");

	if (err != 0)
		return false;

	return true;
}


void CBaseLogger::SetFile(CString path)
{
	this->path = path;
	open();
}


void CBaseLogger::logInfo(wchar_t* message)
{
	SYSTEMTIME st = { 0 };
	DWORD written = 0;
	size_t len = wcsnlen_s(message, (SIZE_MAX - 1)) + 100; // extra characters for storin g null-terminating character and additional data
	wchar_t* buffer = new wchar_t[len];
	
	//GetSystemTime(&st);
	GetLocalTime(&st);

	wmemset(buffer, 0, len);

	// format buffer
	wsprintf(buffer, L"%02d.%02d.%d %02d:%02d:%02d.%03d - %s\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, message);

	if (use_console && (console != 0))
		WriteConsole(console, buffer, len, &written, NULL);

	if (file != 0)
		fwprintf_s(file, L"%s", buffer);

	delete[] buffer;
}

void CBaseLogger::logInfo(CString message)
{
	logInfo((wchar_t*)message.GetBuffer());
}

void  CBaseLogger::logError(wchar_t * message, long error)
{
	SYSTEMTIME st = { 0 };
	DWORD written = 0;
	size_t len = wcsnlen_s(message, (SIZE_MAX - 1)) + 100; // extra characters for storin g null-terminating character and additional data
	wchar_t* buffer = new wchar_t[len];

	GetSystemTime(&st);

	wmemset(buffer, 0, len);

	// format buffer
	wsprintf(buffer, L"%02d.%02d.%d %02d:%02d:%02d.%03d - [ERROR] - %s, Error code: %d (0x%X)\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, message, error, error);

	if (use_console && (console != 0))
		WriteConsole(console, buffer, len, &written, NULL);


	if (file != 0)
		fwprintf_s(file, L"%s", buffer);
	
	delete[] buffer;
}


void  CBaseLogger::logError(CString message, long error)
{
	logError((wchar_t*)message.GetBuffer(), error);
}


void CBaseLogger::open()
{
	int err = 0; // use int instead of weird errno_t which is just simple int itself

	if (path.length() == 0)
		return;

	_wfopen_s(&file, path.GetBuffer(), L"a");
}

void CBaseLogger::close()
{
	if (file == 0)
		return;
	
	fflush(file);
	fclose(file);
	file = 0;
}

void CBaseLogger::init()
{
	path = L"\0";
	file = 0;
	use_console = true;

	AttachConsole(-1);

	console = GetStdHandle(STD_OUTPUT_HANDLE);
	
	if (console == NULL || console == INVALID_HANDLE_VALUE)
		console = 0;
}