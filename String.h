#pragma once
#include "ostream"
#include "framework.h"

class CString
{
public:
	CString();
	CString(const CString&);  						// copy constructor
	CString(CString&&) noexcept; 					// move constructor
	CString(const wchar_t* buffer);					// copy constructor
	CString(const __int64 value);					// copy constructor
	//CString(wchar_t* &&buffer);					// move constructor
	~CString();

	const CString operator+(const CString&) const;	// concatenation
	const CString operator+(const wchar_t*) const;  // concatenation
	const CString operator+(const wchar_t)  const;  // concatenation
	
	CString& operator=(const CString&);  			// copy assignment
	CString& operator=(const wchar_t*);  			// copy assignment
	CString& operator=(const wchar_t);  			// copy assignment
	CString& operator=(const __int64);				// copy assignment
	CString& operator=(CString&&); 					// move assignment
	//CString& operator=(wchar_t*&& string);		// move assignment
	CString& operator+=(const CString&);
	CString& operator+=(const wchar_t*);
	CString& operator+=(const wchar_t);
	CString& operator+=(const __int64);
	bool operator==(const CString&);
	
	CString GetSubString(size_t start, size_t len);	// Returns new object containing substring of the String
	CString Substitute(const wchar_t* pattern, const wchar_t* new_block);

	const wchar_t* GetBuffer() { return buffer; };

	size_t length() { return len; }
private:
	wchar_t* buffer;
	size_t len;
	size_t limit;
protected:
	void Init();
	bool Alloc(size_t);
	void Delete();
};
