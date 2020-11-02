#include "stdafx.h"

#include "memmgr.h"

using namespace std;

int MemoryManager::GetPoolIndex( size_t& size )
{
	size += 4; // guard DWORD
	size_t blockCount = (size / BLOCK_SIZE);
	if ((size % BLOCK_SIZE) > 0)
		blockCount++;
	if (blockCount < MIN_BLOCK_COUNT)
		blockCount = MIN_BLOCK_COUNT;
	size = blockCount * BLOCK_SIZE;
	blockCount -= MIN_BLOCK_COUNT;
	_ASSERT(blockCount < 256);
	return (int)blockCount;
}

void* MemoryManager::memAlloc( size_t size )
{
	int ind = GetPoolIndex(size);
	return allocateEx(ind, size);
}

void* MemoryManager::memAlloc( size_t size, int block, const char * filename, int line )
{
	int ind = GetPoolIndex(size);
	return allocateEx(ind, size);
}

void* MemoryManager::allocateEx( int ind, size_t newSize )
{
	if (ind >= (int)MemoryPoolList.size())
		MemoryPoolList.resize(ind + 1);
	MemPoolData& data = MemoryPoolList[ind];
	if(data.FreeBlocks.empty())
		InitialisePtrList(data, newSize);
	data.nBlockCount++;
	unsigned char* blockPtr = data.FreeBlocks.front();
	*(blockPtr - 2) = ind;
	*(blockPtr - 1) = 0; //block is no longer free
	data.FreeBlocks.pop_front();
	return blockPtr;
}

void MemoryManager::InitialisePtrList( MemPoolData& data, size_t size )
{
	unsigned char* pData = static_cast<unsigned char*>(malloc(size * POOL_SIZE));
	data.PtrList.push_back(pData);
	int ind = (int)data.PtrList.size() - 1;

	unsigned char* guardByteStart = data.PtrList[ind];
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		*guardByteStart = 0xde;
		guardByteStart++; 
		*guardByteStart = ind;
		guardByteStart++;
		*guardByteStart = (unsigned char) size;		//sizeof block
		guardByteStart++;
		*guardByteStart = 1;		//block  available
		guardByteStart++;
		data.FreeBlocks.push_back(guardByteStart);
		guardByteStart += (size - 4);
	}
}

void MemoryManager::memFree( void* object, size_t size )
{
	int ind = GetPoolIndex(size);
	freeEx(object, ind);
}

void MemoryManager::memFree( void* object, size_t size, int block, const char * filename, int line )
{
	int ind = GetPoolIndex(size);
	freeEx(object, ind);
}

void MemoryManager::freeEx( void* object, int ind )
{
	MemPoolData& data = MemoryPoolList[ind];

	unsigned char* pData = static_cast<unsigned char*>(object);

	unsigned char indFromData = *(pData - 2);
	_ASSERT(ind == indFromData);
	data.FreeBlocks.push_back(pData);
	_ASSERT(data.nBlockCount > 0);
	data.nBlockCount--;

	unsigned char* guardByteStart = pData - 4;
	_ASSERT(*guardByteStart == 0xde);
	guardByteStart += 3;
	_ASSERT(*guardByteStart == 0);		//block  unavailable
	*guardByteStart = 1;				//block  available
}

MemoryManager::~MemoryManager()
{
	int i = 0;
	size_t totalSize = 0;
	for (; i < (int)MemoryPoolList.size(); i++)
	{
		size_t elSize = (i + MIN_BLOCK_COUNT) * BLOCK_SIZE;
		totalSize += elSize * MemoryPoolList[i].PtrList.size() * POOL_SIZE;
		MemoryPoolList[i].FreeBlocks.clear();
		std::vector<unsigned char*>::iterator pit = MemoryPoolList[i].PtrList.begin();
		for (; pit != MemoryPoolList[i].PtrList.end(); ++pit)
		{
			::free(*pit);
		}
	}
	DEBUG_MESSAGE(_T("Total memory: %d Kb\n"), totalSize / 1024);
}
