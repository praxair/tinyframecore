#pragma once
#include "framework.h"
#include "ostream"
#include "BaseList.h"
#include "String.h"

#define JSON_UNDEFINED -1
#define JSON_NULL		0
#define JSON_STRING		1
#define JSON_OBJECT		2
#define JSON_ARRAY		3

typedef struct jsonParameter {
	struct jsonNode*	owner;			// link to parent node
	wchar_t*			name;			// name
	void*				value;			// pointer to a values
	int					type;			// value type
	size_t              index;			// internal parametr's index withia Node's array of parameters
}jsonParameter;

typedef struct jsonNode {
	jsonParameter*  uplink;				// uplink to parent node's parameter containing the node
	jsonParameter** parameter;			// array of parameter
	size_t			parameters_count;	// count of parameters
	int				type;				// wchar_t* or jsonNode*
}jsonNode;

class CBaseSettings
{
public:
	CBaseSettings();
	CBaseSettings(const wchar_t*);
	~CBaseSettings();

	bool ReadSettings(const wchar_t*);
	bool WriteSettings(const wchar_t*);

	jsonParameter* GetParameter(const wchar_t* query);

	bool SetArrayPrefix(const wchar_t*);
	bool SetLevelIndent(const wchar_t*);
	const wchar_t* GetArrayPrefix() { return array_prefix; };
	const wchar_t* GetLevelIndent() { return indent; };

	jsonNode* AddNode(int);
	jsonParameter* AddParameter(jsonNode*, const wchar_t* name);
	jsonParameter* FindParameter(jsonNode* owner, const wchar_t* name);
	bool SetValue(jsonParameter* parameter, const wchar_t* value);
	bool SetValue(jsonParameter* parameter, jsonNode* value);
	
	long long GetLongLong(jsonParameter*, long long substitute);
	bool GetBool(jsonParameter*, bool substitute);
	unsigned long long GetULongLong(jsonParameter*, unsigned long long substitute);
	double GetDouble(jsonParameter*, double substitute);
	const wchar_t* GetWchar(jsonParameter*, const wchar_t* substitute);
private:
	wchar_t*	array_prefix;
	wchar_t*    indent;
	wchar_t*	path;
	jsonNode**	node;
	size_t		nodes_count;
	
	int Parse(CString &);
	
	void Init();
	void SavePath(const wchar_t*);
	void Clear();
/*
	jsonNode* AddNode(int type);
	jsonParameter* AddParameter(jsonNode* owner, const wchar_t * name);
	jsonParameter* FindParameter(jsonNode* owner, const wchar_t* name);
	bool SetValue(jsonParameter* parameter, const wchar_t* value);
	bool SetValue(jsonParameter* parameter, jsonNode* value);
*/
	bool IsArrayIndex(const wchar_t*);
	__int64 GetArrayIndex(const wchar_t*);
};
