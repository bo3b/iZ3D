#ifndef __MEMMGR_H_
#define __MEMMGR_H_

#include <vector>
#pragma warning ( push )
#pragma warning ( disable: 4217 )
#include <list>
#pragma warning ( pop )

#ifdef FINAL_RELEASE
const int MIN_BLOCK_COUNT = 2;
#else
const int MIN_BLOCK_COUNT = 4;
#endif
const int BLOCK_SIZE = 16;
const int POOL_SIZE = 64; //number of elements in a single pool
//can be chosen based on application requirements 

struct MemPoolData
{
	MemPoolData() : nBlockCount(0) { }
	std::vector<unsigned char*>	PtrList;
	std::list<unsigned char*>	FreeBlocks;
	int							nBlockCount;
};

class MemoryManager
{
private:
	std::vector<MemPoolData>	MemoryPoolList;
	static int GetPoolIndex( size_t& size );
	void* allocateEx( int ind, size_t size );
	void freeEx( void* object, int ind );
	void InitialisePtrList( MemPoolData& data, size_t size );
public: 
	MemoryManager( ) {}
	~MemoryManager( );
	void* memAlloc( size_t size );
	void* memAlloc( size_t size, int block, const char * filename, int line );
	void  memFree( void* pAddr, size_t size );
	void  memFree( void* pAddr, size_t size, int block, const char * filename, int line );
};

#endif