#include "String.h"
#include "wchar.h"
#include "limits.h"


CString::CString()
{
	Init();
}

// copy constructor
CString::CString(const CString& string)
{
	Init();

	if (Alloc(string.len))
		wmemcpy(buffer, string.buffer, len);
}


// move constructor
CString::CString(CString&& string) noexcept
{
	Init();

	buffer = string.buffer;
	len = string.len;

	string.buffer = 0; // to prevent deleteing in original destructor
}


CString::~CString()
{
	Delete();
}


// copy constructor from wchar_t string
CString::CString(const wchar_t* string)
{
	size_t newLen = 0; 

	Init();
	
	newLen = wcsnlen_s(string, limit);

	if (newLen == 0)
		return;

	if (Alloc(newLen))
		wmemcpy(buffer, string, newLen);
}


CString::CString(const __int64 value)
{
	size_t size = 32;
	size_t newLen = 0;

	Init();

	buffer = new wchar_t[size];

	if (buffer == 0)
		return;

	wmemset(buffer, 0, size);
	newLen = swprintf(buffer, size, L"%d", value);
}


/*
CString::CString(wchar_t*&& string)
{
	buffer = string;
	len = wcsnlen_s(string, limit);

	string = 0;
}
*/


// move assignment
CString& CString::operator=(CString&& string)
{
	if (this == &string)
		return *this;

	buffer = string.buffer;
	len = string.len;

	string.buffer = 0; // to prevent delete the buffer on string object's destructor call
	return *this;
}


// copy assignment
CString& CString::operator=(const CString& string)
{
	if (this == &string)
		return *this;

	Delete();

	if (Alloc(string.len))
		wmemcpy(buffer, string.buffer, string.len);

	return *this;
}


// copy assignment
CString& CString::operator=(const wchar_t* string)
{
	size_t newLen = wcsnlen_s(string, limit);

	if (string == buffer)
		return *this;

	Delete();

	if (Alloc(newLen))
		wmemcpy(buffer, string, newLen);

	return *this;
}

// copy assignment
CString& CString::operator=(const wchar_t character)
{
	Delete();

	if (Alloc(sizeof(wchar_t)))
		wmemcpy(buffer, &character, sizeof(wchar_t));

	return *this;
}


CString& CString::operator=(const __int64 value)
{
	size_t size = 32;
	size_t written = 0;

	Delete();
	
	buffer = new wchar_t[size];
	
	if (buffer == 0)
		return *this;

	wmemset(buffer, 0, size);
	written = swprintf(buffer, size, L"%d", value);

	return *this;
}


CString& CString::operator+=(const __int64 value)
{
	CString tmp_str;
	tmp_str = value;

	return CString(*this) += tmp_str;
}

/*
// move assignment
CString& CString::operator=(wchar_t* &&string)
{
	size_t newLen = wcsnlen_s(string, limit);

	if (string == buffer)
		return *this;

	Delete();

	len = newLen;
	buffer = string;

	string = 0;

	return *this;
}
*/


// overloading 
// two CString objects
const CString CString::operator+(const CString& string) const
{
	return CString(*this) += string;
}


// overloading 
// wchar_t * string and CString object
const CString CString::operator+(const wchar_t* string) const
{
	return CString(*this) += string;
}


// overloading 
// wchar_t character and CString object
const CString CString::operator+(const wchar_t character)  const
{
	return CString(*this) += character;
}


// overloading +=
CString& CString::operator+=(const CString& string)
{
	return (*this) += (string.buffer);
}


CString& CString::operator+=(const wchar_t* string)
{
	size_t tmpLen = wcsnlen_s(string, limit);
	wchar_t * tmpBuffer = 0;

	// check if string has appropriate length
	if ((limit - len) < (tmpLen + 1))
		return *this;

	tmpBuffer = new wchar_t [len + tmpLen + 1];

	if (tmpBuffer == 0)
		return *this;

	wmemset(tmpBuffer, 0, len + tmpLen + 1);

	wmemcpy(tmpBuffer, buffer, len);
	wmemcpy(tmpBuffer + len, string, tmpLen);

	delete[]buffer;

	buffer = tmpBuffer;
	len += tmpLen;

	return *this;
}


CString& CString::operator+=(const wchar_t character)
{
	wchar_t* tmpBuffer = 0;

	// check if string has appropriate length
	if ((limit - len) < 2) // 2 = 1 passed character + 1 null-terminating character
		return *this;

	tmpBuffer = new wchar_t[len + 2]; // 2 = 1 passed character + 1 null-terminating character

	if (tmpBuffer == 0)
		return *this;

	wmemset(tmpBuffer, 0, len + 2); // 2 = 1 passed character + 1 null-terminating character

	wmemcpy(tmpBuffer, buffer, len);
	wmemcpy(tmpBuffer + len, &character, 1);

	delete[]buffer;

	buffer = tmpBuffer;
	len++;

	return *this;
}

// Returns new object containing substring of the String
CString CString::GetSubString(size_t start, size_t len)
{
	wchar_t* tmpBuffer = new wchar_t[len + 1];
	wmemset(tmpBuffer, 0, len + 1);
	wmemcpy(tmpBuffer, buffer + start, len);
	return CString((const wchar_t*)tmpBuffer);
}

CString CString::Substitute(const wchar_t* pattern, const wchar_t* new_block)
{
	CString tmpBuffer;
	wchar_t* tmp = 0;
	size_t start = 0;
	size_t length = wcslen(pattern);

	if (len == 0)
		return tmpBuffer;

	for (size_t i = 0; i < len; i++) {
		if (wcsncmp(buffer + i, pattern, length) == 0) {
 			tmp = new wchar_t[i - start + 1];
			if (tmp != 0) {
				wmemset(tmp, 0, i - start + 1);
				wmemcpy(tmp, buffer + start, i - start);
				tmpBuffer += tmp;
				tmpBuffer += new_block;
				delete[]tmp;
			}
			start = length + i ;
		}
	}
	if (start < len) {
		tmpBuffer += buffer + start;
	}

	return tmpBuffer;
}

void CString::Init()
{
	buffer = 0;
	len = 0;
	limit = SIZE_MAX - 1;
}


// Allocates or reallocates internal buffer
bool CString::Alloc(size_t newLen)
{
	if (newLen > limit)
		return false;

	Delete();

	buffer = new wchar_t[newLen + 1];
	
	if (buffer == 0)
		return false;
	
	wmemset(buffer, 0, newLen + 1);
	len = newLen;

	return true;
}


void CString::Delete()
{
	if (buffer != 0)
	{
		delete[] buffer;
		buffer = 0;
		len = 0;
	}
}


bool CString::operator==(const CString& string)
{
	if (len != string.len)
		return false;

	if ((buffer == 0) || (string.buffer == 0))
		return false;

	if (wcscmp(buffer, string.buffer) == 0)
		return true;

	return false;
}
