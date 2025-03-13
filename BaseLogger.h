#pragma once
#include "framework.h"
#include "ostream"
#include "String.h"

class CBaseLogger
{
public:
	CBaseLogger();
	CBaseLogger(wchar_t*);
	CBaseLogger(CString);
	~CBaseLogger();
	
	void SetFile(CString);

	bool Clear();

	void UseConsole(bool use);

	virtual void logInfo(CString message);
	virtual void logInfo(wchar_t* message);
	virtual void logError(CString message, long error);
	virtual void logError(wchar_t * message, long error);
private:
	CString path;
	FILE* file;
	bool use_console;
	HANDLE console = 0;

	void init();
	void open();
	void close();
};

