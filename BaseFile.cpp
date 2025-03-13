#include "BaseFile.h"
#include "BaseLogger.h"

CBaseFile::CBaseFile(const wchar_t* path)
{
	size_t length = 0;
	
	file = 0;
	CBaseFile::path = 0;

	if (path != 0) {
		length = wcslen(path);
		CBaseFile::path = new wchar_t[length + 1];
		wmemset(CBaseFile::path, 0, length + 1);
		wmemcpy(CBaseFile::path, path, length + 1);
	}
}

CBaseFile::~CBaseFile()
{
	if (file != 0)
		fclose(file);
	
	if (path != 0)
		delete[]path;
}

size_t CBaseFile::ReadAll(wchar_t*& buffer)
{
	const size_t size = 10;		// including null-terminating character;
	size_t buffer_len = 0;
	wchar_t tmp[size] = { 0 };
	CString tmp_buffer;

	if ((buffer == 0) || (path == 0))
		return 0;

	if (_wfopen_s(&file, this->path, L"rt,ccs=UTF-8") != 0) //UNICODE
		return 0;

	while (fgetws(tmp, size, file) != NULL) {
		tmp_buffer += CString(tmp);
	}

	fclose(file);

	buffer_len = tmp_buffer.length();

	delete[]buffer;

	buffer = new wchar_t[buffer_len + 1];

	if (buffer == 0)
		return 0;

	wmemset(buffer, 0, buffer_len + 1);
	wmemcpy(buffer, tmp_buffer.GetBuffer(), buffer_len);

	return buffer_len;
}

bool CBaseFile::WriteAll(const wchar_t* buffer)
{
	int return_value = 0;

	if (path == 0)
		return 0;

	if (_wfopen_s(&file, this->path, L"wt,ccs=UTF-8") != 0) //ccs=UTF-8 == UNICODE
		return false;
	
	//fseek(file, 0L, SEEK_SET);// Remove BOM at the beginning

	return_value = fputws(buffer, file);

	fclose(file);

	return (return_value > 0);
}