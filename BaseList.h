#pragma once
#include "framework.h"

#define DEFAULT_CHUNK_FACTOR 10

class CBaseList
{
public:
	CBaseList();
	CBaseList(size_t item_size);
	~CBaseList();

	bool SetChunkFactor(unsigned __int64);
	unsigned __int64 GetChunkFactor() { return chunk_factor;};
	bool SetItemSize(size_t);
	size_t GetItemSize() { return item_size; };

	void Clear();
	bool Add(const void * item);
	bool Set(unsigned __int64 item_index, const void* item);
	bool Swap(unsigned __int64 item_index_1, unsigned __int64 item_index_2);
	bool Delete(unsigned __int64 item_index);

	void* Get(unsigned __int64 item_index);
	
	unsigned __int64 GetItemsCount() { return items_count; };
private:
	void*  data;
	size_t       item_size;
	unsigned __int64 items_count;
	unsigned __int64 chunk_factor;
	unsigned __int64 chunks_count;

	void clear();
	void init();
	bool increase(unsigned __int64 new_items_count);
	void decrease();
};