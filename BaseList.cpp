#include "BaseList.h"
#include "limits.h"

CBaseList::CBaseList()
{
	init();
}

CBaseList::CBaseList(size_t item_size)
{
	init();

	if (item_size > 0)
		this->item_size = item_size;
}


CBaseList::~CBaseList()
{
	clear();
}


/*
	Parameter:	size_t chunk_factor
				Chunk is a number of items
				If new chunk factor is not equal to existing one then the whole list are clear before new chunk factor set
	Returns:
				true if new chunk factor has been set. false otherwise
*/
bool CBaseList::SetChunkFactor(unsigned __int64 chunk_factor)
{
	if (chunk_factor == 0)
		return false;

	if (this->chunk_factor != chunk_factor)
		clear();

	this->chunk_factor = chunk_factor;

	return true;
}

/*
* Parameter:	size_t item_size
*				If new item size is not equal to existing one than whole list are clear before new item size set
* Returns:
*				true if new item size has been set. false otherwise
*/
bool CBaseList::SetItemSize(size_t item_size)
{
	if (item_size == 0)
		return false;

	if (this->item_size != item_size)
		clear();

	this->item_size = item_size;

	return true;
}


void CBaseList::Clear()
{
	clear();
}

/*
* Parameters:	pointer to an item
* Returns:		If success the function returna index of new item within the list. First index is 0.
*				if failed, the function returns -1	
*/
bool CBaseList::Add(const void * item)
{
	if (item == 0)
		return false;

	if (!increase(1))
		return false;

	memcpy((void*)((unsigned __int64)data + (items_count * item_size)), item, item_size);

	return items_count++;
}


bool CBaseList::Set(unsigned __int64 item_index, const void* item)
{
	if ((item_index >= items_count) || (item == 0) || (data == 0))
		return false;

	memcpy((void*)((unsigned __int64)data + (item_index * item_size)), item, item_size);
	
	return true;
}


bool CBaseList::Swap(unsigned __int64 item_index_1, unsigned __int64 item_index_2)
{
	void* tmp_data = 0;

	if ((item_index_1 == item_index_2) || (item_index_1 >= items_count) || (item_index_2 >= items_count))
		return false;

	if ((tmp_data = new char[item_size]) == 0)
		return false;

	// save item will be overwritten
	memcpy(tmp_data, (void*)((unsigned __int64)data + (item_index_1 * item_size)), item_size);

	// overwright saved item	
	memcpy((void*)((unsigned __int64)data + (item_index_1 * item_size)), (void*)((unsigned __int64)data + (item_index_2 * item_size)), item_size);

	// restore saved item	
	memcpy((void*)((unsigned __int64)data + (item_index_2 * item_size)), tmp_data, item_size);

	delete[] tmp_data;
	
	return true;
}


bool CBaseList::Delete(unsigned __int64 item_index)
{
	if ((item_index >= items_count))
		return false;

	if ((items_count > 1) && (item_index <= (items_count - 1)))
	{
		memmove((void*)((unsigned __int64)data + (item_index * item_size)),
			    (void*)((unsigned __int64)data + ((item_index + 1) * item_size)),
			         item_size * (items_count - item_index - 1));
		
		items_count--;
		decrease();
	}
	else
	{
		Clear();
	}

	return true;
}


void* CBaseList::Get(unsigned __int64 item_index)
{
	if (item_index >= items_count)
		return 0;

	return (void*)((unsigned __int64)data + (item_index * item_size));
}


void CBaseList::init()
{
	data         = 0;
	item_size    = sizeof(__int64);
	items_count  = 0;
	chunk_factor = DEFAULT_CHUNK_FACTOR;
	chunks_count = 0;
}


void CBaseList::clear()
{
	if (data == 0)
		return;

	delete[] data;
	
	data = 0;
	items_count = 0;
	chunks_count = 0;
}


// should be called before any item added to the list
bool CBaseList::increase(unsigned __int64 items_count)
{
	void* new_data          = 0;
	size_t new_data_size    = 0;
	size_t new_chunks_count = 0;
	size_t new_items_count  = 0;

	// prevent overflow
	if ((ULLONG_MAX - this->items_count) < new_items_count)
		return false;

	new_items_count = this->items_count + new_items_count;

	// prevent overflow
	if (((unsigned __int64)SIZE_MAX - (this->items_count * item_size)) < (new_items_count * item_size))
		return false;

	new_data_size = new_items_count * item_size;

	// check if existing room is enough yet
	if (new_data_size <= (chunks_count * item_size))
		return true;

	new_chunks_count = (new_data_size % (chunk_factor * item_size) > 0) ? (new_data_size / (chunk_factor * item_size) + 1) : (new_data_size / (chunk_factor * item_size));

	new_data = new char[new_chunks_count * chunk_factor * item_size];

	if (new_data == 0)
		return false;

	memset(new_data, 0, (new_chunks_count * chunk_factor * item_size));

	if (data != 0)
		memcpy(new_data, data, (items_count * item_size));

	delete[] data;

	data = new_data;
	chunks_count = new_chunks_count;

	return true;
}


// should be called after one or more item(s) deleted
// shrinks allocated memory to actual size
void CBaseList::decrease()
{
	void* new_data = 0;
	size_t new_chunks_count = 0;

	if (items_count == 0)
		clear();

	new_chunks_count = ((items_count * item_size) % (chunk_factor * item_size) > 0) ? ((items_count * item_size) / (chunk_factor * item_size) + 1 ) : ((items_count * item_size) / (chunk_factor * item_size));

	if (new_chunks_count == chunks_count)
		return;

	new_data = new char[new_chunks_count * chunk_factor * item_size];
	
	if (new_data == 0)
		return;
		
	memset(new_data, 0, (new_chunks_count * chunk_factor * item_size));

	if (data != 0)
		memcpy(new_data, data, (items_count * item_size));

	delete[] data;

	data = new_data;
	chunks_count = new_chunks_count;
}