#pragma once
#include "framework.h"
#include "String.h"
#include "ostream"

class CBaseFile
{
public:
	CBaseFile(const wchar_t* path);
	~CBaseFile();

	size_t ReadAll(wchar_t*& buffer);
	bool WriteAll(const wchar_t* buffer);
private:
	FILE* file;
	wchar_t* path;
};