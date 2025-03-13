#include "BaseSettings.h"
#include "BaseList.h"

CBaseSettings::CBaseSettings()
{
	Init();
}

CBaseSettings::CBaseSettings(const wchar_t* path)
{
	Init();
	SavePath(path);
}

CBaseSettings::~CBaseSettings()
{
	if (path != 0)
		delete[] path;

	if (array_prefix != 0)
		delete[] array_prefix;

	if (indent != 0)
		delete[] indent;

	Clear();
}


void CBaseSettings::Init()
{
	path = 0;
	node = 0;
	nodes_count = 0;

	array_prefix = new wchar_t[2];

	if (array_prefix != 0)	{
		wmemset(array_prefix, 0, 2);
		wmemcpy(array_prefix, L"#", 1);
	}

	indent = new wchar_t[4];
	if (indent != 0) {
		wmemset(indent, 0, 4);
		wmemcpy(indent, L"   ", 3);
	}
}



bool CBaseSettings::SetArrayPrefix(const wchar_t * prefix)
{
	wchar_t* new_prefix = 0;
	size_t length = 0;

	if (prefix == 0)
		return false;

	length = wcslen(prefix);

	new_prefix = new wchar_t[length + 1];
	if (new_prefix == 0)
		return false;

	wmemcpy(new_prefix, prefix, length + 1);

	if (array_prefix != 0)
		delete[]array_prefix;
	
	array_prefix = new_prefix;

	return true;
}


bool CBaseSettings::SetLevelIndent(const wchar_t* indent)
{
	wchar_t* new_indent = 0;
	size_t length = 0;

	if (indent == 0)
		return false;

	length = wcslen(indent);

	new_indent = new wchar_t[length + 1];
	
	if (new_indent == 0)
		return false;

	wmemcpy(new_indent, indent, length + 1);

	if (this->indent != 0)
		delete[]this->indent;

	this->indent = new_indent;

	return true;
}


void CBaseSettings::SavePath(const wchar_t* path)
{
	size_t len = wcsnlen_s(path, SIZE_MAX - 1);

	if (len == 0)
		return;

	if (this->path != 0)
		delete[] (this->path);

	this->path = new wchar_t[len + 1];
	wmemset(this->path, 0, len + 1);
	wmemcpy(this->path, path, len);
}


bool CBaseSettings::ReadSettings(const wchar_t* path)
{
	FILE* file;
	CString buffer;
	const size_t size = 10; // including null-terminating character;
	wchar_t tmp[size] = { 0 };

	if (this->path == 0)
	{
		if (path == 0)
			return false;
	}

	SavePath(path);

	if (_wfopen_s(&file, this->path, L"rt,ccs=UTF-8") != 0)
		return false;

	while (fgetws(tmp, size, file) != NULL) {
		buffer += tmp;
	}

	fclose(file);
	
	if (Parse(buffer) < 0)
		MessageBox(NULL, L"An unexpected error occured while parsing", L"Parsing", MB_OK & MB_ICONERROR);

	return true;
}

/*
* Function Parse
*
* Parameters:
*	CString& buffer			- type of node. Either JSON_OBJECT or JSON_ARRAY
*
* Return value:
*	If function succeeds, the return value is 0.
*   If function fails, the return value is none zero.
*
* Remarks:
*	Function parses passed string and tries interpretate it as a valid JSON object then parses it.
*	The function intendly written recursion free.
*/
int CBaseSettings::Parse(CString& buffer)
{
	const wchar_t*	data = buffer.GetBuffer();
	jsonParameter*	current_parameter = 0;
	jsonNode*		current_node = 0;

	size_t index = 0;

	bool f_str  = false;
	bool f_txt  = false;
	bool f_esc  = false;

	CString text;

	// skip spaces if any
	while ((data[index] == ' ') || (data[index] == '\t') || (data[index] == '\n')) {
		index++;
	}

	// check for '[' or '{' first
	if ((data[index] != '[') && ((data[index] != '{')))
		return -1;

	// seems to be a json file
	// so try to parse it
	for (size_t i = index; i < buffer.length(); i++)
	{
		//character[0] = data[i];
		switch (data[i])
		{
		case ' ':
		case '\t':
		case '\n':
			// flags management
			if (f_str)
				text += data[i];

			f_txt = false;
			f_esc = false;
			break;
		//case '\\':
		//	// flags management
		//	if (f_str) {
		//		text += data[i];
		//		f_esc = true;
		//	}
		//	break;
		case '"':
			// flags management
			if (f_esc && f_str) {
				text += data[i];
				f_esc = false;
				break;
			}

			f_str = !f_str;
			break;
		case ',':
			// flags management
			if (f_str) {
				text += data[i];
				break;
			}
			
			f_txt = false;
			f_esc = false;

			if (text.length() > 0) {
				if (current_node->type == JSON_ARRAY)
					current_parameter = AddParameter(current_node, 0);

				SetValue(current_parameter, text.GetBuffer());
				text = L"";
			}
			break;
		case ':':
			// flags management
			if (f_str) {
				text += data[i];
				break;
			}

			f_txt = false;
			f_esc = false;

			if (text.length() > 0) {
				current_parameter = AddParameter(current_node, text.GetBuffer());
				text = L"";
			}

			break;
		case '{':
				// flags management
				if (f_str) {
					text += data[i];
					break;
				}

				f_txt = false;
				f_esc = false;
				
				if (current_node == 0) {
					current_node = AddNode(JSON_OBJECT);
				}
				else {
					if (current_node->type == JSON_ARRAY)
						current_parameter = AddParameter(current_node, 0);

					current_node = AddNode(JSON_OBJECT);
					SetValue(current_parameter, current_node);
				}
				break;
			case '}':
				// flags management
				if (f_str) {
					text += data[i];
					break;
				}

				f_txt = false;
				f_esc = false;

				if (text.length() > 0) {
					SetValue(current_parameter, text.GetBuffer());
					text = L"";
				}
				
				if (current_node != 0) {
					if (current_node->uplink != 0)
						current_node = current_node->uplink->owner;
				}
				break;
			case '[':
				// flags management
				if (f_str) {
					text += data[i];
					break;
				}

				f_txt = false;
				f_esc = false;

				if (current_node == 0) {
					current_node = AddNode(JSON_ARRAY);
				}
				else {
					if (current_node->type == JSON_ARRAY)
						current_parameter = AddParameter(current_node, 0);

					current_node = AddNode(JSON_ARRAY);
					SetValue(current_parameter, current_node);
				}
				break;
			case ']':
				// flags management
				if (f_str) {
					text += data[i];
					break;
				}
				f_txt = false;
				f_esc = false;

				if (text.length() > 0){
					current_parameter = AddParameter(current_node, 0);
					SetValue(current_parameter, text.GetBuffer());
					text = L"";
				}
				if (current_node != 0) {
					if (current_node->uplink != 0)
						current_node = current_node->uplink->owner;
				}
				break;
			default:
				if (!f_str)
					f_txt = true;

				text += data[i];
		}
	}
	return 0;
}

/*
* Function AddNode
*
* Parameters:
*	int type				- type of node. Either JSON_OBJECT or JSON_ARRAY
*
* Return value:
*	If function succeeds, the return value is pointer to jsonNode object.
*   If function fails, the return value is 0.
*
* Remarks:
*	Function adds a node to a nodes list. Each JSON array or JSON object represented as node inside array.
*   Type should be set correctly to JSON_ARRAY or JSON_OBJECT. Other types are invalid for a node.
*
*/
jsonNode* CBaseSettings::AddNode(int type)
{
	jsonNode** array   = 0;
	jsonNode* new_node = 0;

	// check for proper type
	if ((type != JSON_OBJECT) && (type != JSON_ARRAY))
		return 0;

	// create a new node
	new_node = new jsonNode;

	if (new_node == 0)
		return 0;

	memset(new_node, 0, sizeof(jsonNode));
	new_node->type = type;

	// allocate memory for new array
	array = new jsonNode * [nodes_count + 1];

	if (array == 0) {
		delete new_node;
		return 0;
	}

	// copy existing data to new array
	memcpy(array, node, nodes_count * sizeof(jsonNode*));
	array[nodes_count] = new_node;

	// delete previous array of pointers to jsonNodes
	if (node != 0)
		delete[]node;

	// fix array of pointers to jsonNodes
	node = array;
	nodes_count++;

	return new_node;
}

/*
* Function AddParameter
*
* Parameters:
*	jsonNode* owner			- pointer to a jsonNode object. Non-zero
*	const __wchar_t* name	- a string representing parametr's name.
*
* Return value:
*	If function succeeds, the return value is pointer to jsonParameter object.
*   If function fails, the return value is 0.
*
* Remarks:
*	Function adds a parameter to existing node. All parameters are stored within an array inside a node.
*   Name should be set if parameter is an valid JSON Object's parameter. Or should be set to zero in case
*   of JSON array item.
*
*/
jsonParameter* CBaseSettings::AddParameter(jsonNode* owner, const wchar_t* name)
{
	jsonParameter** array = new jsonParameter * [owner->parameters_count + 1];
	jsonParameter* new_parameter = 0;
	size_t length = 0;
	
	// check if we have allocated needed memory
	if (array == 0)
		return new_parameter;

	memset(array, 0, (owner->parameters_count + 1) * sizeof(jsonParameter*));
	
	// create new parameter
	new_parameter = new jsonParameter;

	// check if new_parameter has allocated memory
	if (new_parameter == 0) {
		delete[]array;
		return new_parameter;
	}

	// fill newParameter by default
	memset(new_parameter, 0, sizeof(jsonParameter));
	new_parameter->owner = owner;
	new_parameter->index = owner->parameters_count;

	// save parameter's name if passed
	if (name != 0) {
		length = wcslen(name);
		new_parameter->name = new wchar_t[length + 1];

		if (new_parameter->name != 0)
			wmemcpy(new_parameter->name, name, length + 1);
	}

	// copy existing array to a new location
	if (owner->parameter != 0) {
		memcpy(array, owner->parameter, (owner->parameters_count) * sizeof(jsonParameter*));
		delete[]owner->parameter;
	}
	
	// Add newly created parameter to array
	array[owner->parameters_count] = new_parameter;

	owner->parameter = array;
	owner->parameters_count++;

	return new_parameter;
}

/*
* Function SetValue(1)
*
* Parameters:
*	jsonParameter* parameter- pointer to a jsonParameter object. Non-zero
*	const __wchar_t* value	- a string representing the value. May be zero
*
* Return value:
*	If function succeeds, the return value true.
*   If function fails, the return value false.
*
* Remarks:
*	Function sets strings as value for specified parameter. The same time function
*   sets uplink to parent parameters
*/
bool CBaseSettings::SetValue(jsonParameter* parameter, const __wchar_t* value)
{
	size_t length = 0;
	
	if (parameter == 0)
		return false;

	// check if parameter is already set
	if (parameter->value != 0)
	{
		// delete existing string value
		if (parameter->type == JSON_STRING)
			delete[]parameter->value;
		
		// reset for future manipulations
		parameter->value = 0;
		parameter->type = JSON_NULL;
	}

	if (value == 0)
		return true;

	length = wcslen(value);
	
	parameter->value = new wchar_t [length + 1];

	if (parameter->value == 0)
		return false;

	parameter->type = JSON_STRING;

	memcpy(parameter->value, value, (length + 1) * sizeof(wchar_t));

	return true;
}

/*
* Function SetValue(2)
*
* Parameters:
*	jsonParameter* parameter- pointer to a jsonParameter object. Non-zero.
*	jsonNode* node			- pointer to a jsonNode object. May be zero.
*
* Return value:
*	If function succeeds, the return value true.
*   If function fails, the return value false.
*
* Remarks:
*	Function sets object as value for specified parameter. The same time function
*   sets uplink to parent parameters
*/
bool CBaseSettings::SetValue(jsonParameter* parameter, jsonNode* node)
{
	if (parameter == 0)
		return false;

	// check if parameter is already set
	if (parameter->value != 0)
	{
		// delete existing string value
		if (parameter->type == JSON_STRING)
			delete[]parameter->value;

		// reset parametrs data for future use
		parameter->value = 0;
		parameter->type = JSON_NULL;
	}

	if (node != 0) {
		parameter->value = (void*)node;
		parameter->type = node->type;
		node->uplink = parameter;
	}

	return true;
}

/*
* Function FindParameter
*
* Parameters:
*	jsonNode* node			- pointer to a jsonNode object
*	const wchar_t* name		- a string representing parameter's name to find
*
* Return value:
*	If function succeeds, the return value is pointer to a jsonParameter.
*   If function fails, the return value is 0.
*
* Remarks:
*	Iterates through all parameters in search of specific one by its name.
*/
jsonParameter* CBaseSettings::FindParameter(jsonNode* node, const wchar_t* name)
{
	// node must be a valid object
	if (node == 0)
		return (jsonParameter*)0;

	for (size_t i = 0; i < node->parameters_count; i++)	{
		if (wcscmp(name, (const wchar_t*)(node->parameter[i]->name)) == 0)
			return node->parameter[i];
	}

	return (jsonParameter*)0;
}

/*
* Function Clear
* 
* Parameters:
*	none
* 
* Return value:
*   none
* 
* Remarks:
*	Cleans heap. Frees allocated memory.
*/
void CBaseSettings::Clear()
{
	if (node == 0)
		return;

	for (size_t i = 0; i < nodes_count; i++)
	{
		if (node[i]->parameter != 0)
		{
			for (size_t j = 0; j < node[i]->parameters_count; j++)
			{
				if (node[i]->parameter[j]->name != 0)
					delete[]node[i]->parameter[j]->name;

				if ((node[i]->parameter[j]->value != 0) && (node[i]->parameter[j]->type == JSON_STRING))
					delete[]node[i]->parameter[j]->value;

				if (node[i]->parameter[j] != 0)
					delete node[i]->parameter[j];
			}
		}
		delete[]node[i]->parameter;
	}

	return;
}

/*
* Function GetParameter
* 
* Parameters:
*	const wchar_t* query	- a string representing query.
* 
* Return value:
*	If function succeeds, the return value is pointer to a jsonParameter.
*   If function fails, the return value is 0.
* 
* Remarks:
*   Qery must contain sequence of parameters' names or array indeces
*   separated by commas ('.'). Array index must have specific prefix (by default is '#').
*   Array index prefix could be set by call SetArrayPrefix(const wchar_t*).
*   First item within an array has index 0.
* 
* Examples:
*   jsonParameter * parameter = GetParameter (L"quiz.maths.q2.options.#2");
* 
*	here "quiz", "maths", "q2" and "options" are names of parameters of nested objects
*   while "#2" - is index to third item within an nested array.
*/
jsonParameter* CBaseSettings::GetParameter(const wchar_t* query)
{
	wchar_t*		chunk				= 0;
	wchar_t*		buffer				= 0;
	size_t			length				= 0;
	bool			f_new_chunk			= false;
	jsonNode*		current_node		= 0;
	jsonParameter*	current_parameter	= 0;
	__int64			array_index			= -1;

	if ((query == 0) || (node == 0))
		return (jsonParameter*)0;

	// creatre local copy of query to deal with
	length = wcslen(query);

	buffer = new wchar_t[length + 1];
	
	if (buffer == 0)
		return (jsonParameter*)0;

	wmemcpy(buffer, query, length + 1);
	
	// set first chunk
	chunk = buffer;

	// set current node
	current_node = node[0];

	// process buffer
	for (size_t i = 0; i < length; i++)
	{
		// substitute every '.' in query with zero to split to chunks
		if (buffer[i] == '.') {
			buffer[i] = 0;
			f_new_chunk = true;
			continue;
		}

		// last query chunks need that check
		if (i == (length - 1))
			f_new_chunk = true;

		// process chunk
		if (f_new_chunk)
		{
			// analyze chunk
			// check if chunk is array index
 			if (IsArrayIndex(chunk))
			{
				// check if current node is JSON_ARRAY
				if (current_node->type != JSON_ARRAY) {
					delete[]buffer;
					return (jsonParameter*)0;
				}

				// convert chunk into array index
				array_index = GetArrayIndex(chunk);
				
				// check array index for validity
				if ((array_index >= current_node->parameters_count) || (array_index < 0)) {
					delete[]buffer;
					return (jsonParameter*)0;
				}

				// just get parameter by array index
				current_parameter = current_node->parameter[array_index];

				// fix current node if current parameter points to an JSON_ARRAY or JSON_OBJECT
				if ((current_parameter->type == JSON_ARRAY) || (current_parameter->type == JSON_OBJECT))
					current_node = (jsonNode*)current_parameter->value;
			}
			else
			{
				// check if current node is JSON_OBJECT
				if (current_node->type != JSON_OBJECT) {
					delete[]buffer;
					return (jsonParameter*)0;
				}
				
				// try to findt parameter by its name
				current_parameter = FindParameter(current_node, chunk);

				// check if we found the parameter
				if (current_parameter == 0) {
					delete[]buffer;
					return (jsonParameter*)0;
				}

				// fix current node if current parameter points to an JSON_ARRAY or JSON_OBJECT
				if ((current_parameter->type == JSON_ARRAY) || (current_parameter->type == JSON_OBJECT))
					current_node = (jsonNode*)current_parameter->value;
			}

			// reset f_new_chunk
			f_new_chunk = false;

			chunk = buffer + i;
		}
	}
	
	delete[]buffer;

	// return no matter we got
	return current_parameter;
}


bool CBaseSettings::IsArrayIndex(const wchar_t* buffer)
{
	if ((buffer == 0) || (array_prefix == 0))
		return false;

	if (wcsncmp(buffer, array_prefix, wcslen(array_prefix)) == 0)
		return true;
	
	return false;
}


__int64 CBaseSettings::GetArrayIndex(const wchar_t* buffer)
{
	__int64			index	= -1;
	const wchar_t*	value	= 0;

	if (buffer == 0)
		return false;

	value = buffer + wcslen(array_prefix);

	index = wcstol(value, 0, 10);

	return index;
}

/*
* Function WriteSettings
*
* Parameters:
*	const wchar_t* path		- type of node. Either JSON_OBJECT or JSON_ARRAY
*
* Return value:
*	If function succeeds, the return value is true.
*   If function fails, the return value is false.
*
* Remarks:
*	Function writes settings as a JSON file.
*	The function intendly written recursion free.
*/
bool CBaseSettings::WriteSettings(const wchar_t* path)
{
	FILE*			file				= 0;
	jsonNode*		current_node		= 0;
	jsonParameter*	current_parameter	= 0;
	__int64			index				= 0;
	__int64			indent_count		= 0;
	
	// check if we have something to write down and a specific place to do it
	if ((path == 0) || (node == 0))
		return false;
	
	// check if we have none Empty object (like { } or [ ])
	if (node[0]->parameters_count > 0)
		index = 0;

	// set current node
	current_node = node[0];

	// try to open file to write in it
	if (_wfopen_s(&file, path, L"w") != 0)
		return false;

	// process all settings
	while (current_node != 0)
	{
		// #1
		// Start of a JSON node
		if (index == 0)
		{
			indent_count++;

			if (current_node->type ==  JSON_OBJECT)
				fputws(L"{\n", file);
			else
				fputws(L"[\n", file);
		}
		
		// #2
		// Check for empty Node
		if (current_node->parameter == 0)
		{
			if (current_node->type == JSON_OBJECT)
				fputws(L" }", file);
			else
				fputws(L" ]", file);

			fclose(file);
			return true;
		}

		// #3
		// get parameter and process it
		current_parameter = current_node->parameter[index];

		// write indent
		for (size_t k = 0; k < indent_count; k++)
			fputws(indent, file);

		// Write key name if parameter of Object
		if (current_node->type == JSON_OBJECT) {
			fputws(L"\"", file);
			fputws(current_parameter->name, file);
			fputws(L"\"", file);
			fputws(L" : ", file);
		}
		
		// Write value
		if (current_parameter->value == 0)
			fputws(L"null", file);
		else
		{
			if ((current_parameter->type == JSON_ARRAY) || (current_parameter->type == JSON_OBJECT))
			{
				// Move next node
				current_node = (jsonNode*)current_parameter->value;

				if (current_node->parameters_count == 0) {
					if (current_node->type == JSON_OBJECT)
						fputws(L"{ }", file);
					else
						fputws(L"[ ]", file);
					
					//current_parameter = 0;

					current_node = current_node->uplink->owner;
					//continue;
				}
				else
				{
					current_parameter = current_node->parameter[0];
					index = 0;
					continue;
				}
			}
			else
			{
				// Write value
				fputws(L"\"", file);
				fputws((const wchar_t*)current_parameter->value, file);
				fputws(L"\"", file);
			}
		}

		// Write comma between parameters or values
		if (current_parameter->index < (current_node->parameters_count - 1)) {
		//fputws(L",\n", file);
		}

		// #4
		// End of a JSON node
		if (index == (current_node->parameters_count - 1))
		{
			fputws(L"\n", file);

			// write indent
			indent_count--;
			for (size_t k = 0; k < indent_count; k++)
				fputws(indent, file);

			if (current_node->type == JSON_OBJECT)
				fputws(L"}", file);
			else
				fputws(L"]", file);

			// check if uplink is valid
			if (current_node->uplink == 0)
				break;

			current_parameter = current_node->uplink;
			current_node = current_parameter->owner;
			index = current_parameter->index ;
			
			while (index == (current_node->parameters_count - 1))
			{
				if (current_node->uplink == 0)
				{
					if (current_node->type == JSON_OBJECT)
						fputws(L"\n}", file);
					else
						fputws(L"\n]", file);
					
					fclose(file);
					return true;
				}
				
				// new line
				fputws(L"\n", file);
				
				// write indent
				indent_count--;
				for (size_t k = 0; k < indent_count; k++)
					fputws(indent, file);

				if (current_node->type == JSON_OBJECT)
					fputws(L"}", file);
				else
					fputws(L"]", file);

				current_parameter = current_node->uplink;
				current_node = current_parameter->owner;
				index = current_parameter->index;
			}
		}

		// proceed further
		fputws(L",\n", file);
		current_parameter = current_node->parameter[++index];
	}

	fclose(file);
	return true;
}

// If the converted value falls out of range of corresponding return type, range error occurs and LLONG_MAX or LLONG_MIN is returned.
// If no conversion can be performed,0 is returned. 
long long CBaseSettings::GetLongLong(jsonParameter* parameter, long long substitute)
{
	if ((parameter->type == JSON_STRING) && (parameter->value != 0))
		return wcstoll((wchar_t*)parameter->value, 0, 0);
	
	return substitute;
}

// Return true or false if succeed
// Return substitute if failed
bool CBaseSettings::GetBool(jsonParameter* parameter, bool substitute)
{
	bool result = false;
	size_t length = 0;
	wchar_t* value = 0;

	if ((parameter->type == JSON_STRING) && (parameter->value != 0))
	{
		// make a local copy of passed parameter
		length = wcslen((wchar_t*)parameter->value);
		
		value = new wchar_t[length + 1];

		// check for proper memory allocation
		if (value == 0)
			return substitute;

		// copy parameter's value into local variable
		wmemset(value, 0, length + 1);
		wmemcpy(value, (wchar_t*)parameter->value, length);

		// convert it to lower case first
		_wcslwr_s(value, length + 1);

		// determine holding value
		if (wcscmp(value, L"false") == 0)
			result = false;
		else if (wcscmp(value, L"true") == 0)
			result = true;
		else
			result = substitute;

		// free memory
		delete[]value;

		return result;
	}

	return result;
}

// If the converted value falls out of range of corresponding return type, range error occurs and ULONG_MAX or ULLONG_MAX is returned.
// If no conversion can be performed, 0 is returned. 
unsigned long long CBaseSettings::GetULongLong(jsonParameter* parameter, unsigned long long substitute)
{
	if ((parameter->type == JSON_STRING) && (parameter->value != 0))
		return wcstoull((wchar_t*)parameter->value, 0, 0);

	return substitute;
}

// If the converted value falls out of range of corresponding return type, range error occurs and HUGE_VAL, HUGE_VALF or HUGE_VALL is returned.
// If no conversion can be performed, 0 is returned. 
double CBaseSettings::GetDouble(jsonParameter* parameter, double substitute)
{
	if ((parameter->type == JSON_STRING) && (parameter->value != 0))
		return wcstod((wchar_t*)parameter->value, 0);

	return substitute;
}


const wchar_t* CBaseSettings::GetWchar(jsonParameter* parameter, const wchar_t* substitute)
{
	if ((parameter->type == JSON_STRING) && (parameter->value != 0))
		return (const wchar_t*)parameter->value;

	return substitute;
}