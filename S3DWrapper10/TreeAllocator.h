#pragma once

#include <algorithm>

/** Allocator based on twins algorithm. 
 * Constant allocation/deallocation time. 
 * Internal fragmentation up to half allocated memory.
 */
class tree_allocator :
	public boost::noncopyable
{
private:
	struct chunk
	{
		bool	allocated;
		size_t	available;
	};

public:
	tree_allocator()
	:	minSize_(0)
	,	maxSize_(0)
	,	data_(0)
	,	chunks_(0)
	{
	}

	~tree_allocator()
	{
		_aligned_free(data_);
		data_ = 0;
		delete[] chunks_;
		chunks_ = 0;
		minSize_ = 0;
		maxSize_ = 0;
		numBlocks_ = 0;
	}

	/** Recreate memory storage.
	 * @param minSize - minimum allocatable chunk.
	 * @param maxSize - maximum allocatable chunk.
	 * @param align - memory alignment. 
	 */
	void reallocate(size_t minSize, size_t maxSize, size_t align = 0x10)
	{
		assert(maxSize > 0 && minSize > 0 && (maxSize % minSize) == 0);

		minSize_   = minSize;
		maxSize_   = maxSize;
		numBlocks_ = ((maxSize_ / minSize_) << 1) - 1;
		data_      = (char*)_aligned_malloc(maxSize_, align);
		chunks_    = new chunk[numBlocks_];

		// fill up chunks
		size_t numBlocks = 1;
		size_t i = 0;
		size_t size = maxSize_;
		while (size >= minSize_)
		{
			for (size_t j = 0; j<numBlocks; ++j, ++i) 
			{
				chunks_[i].allocated = false;
				chunks_[i].available = size;
			}
			size >>= 1;
			numBlocks <<= 1;
		}
		/* Memory debug
		std::fill( (char*)data_.get(), (char*)data_.get() + maxSize_, 'f');
		*/
	}

	/** Allocate memory chunk. Return 0 if fails. 
	 * Performs for O(log(maxSize) - log(minSize))
	 */
	void* allocate(size_t size)
	{
		if ( !chunks_ || chunks_[0].available < size ) {
			return 0;
		}

		char* data = data_;
		size_t i = 0;
		size_t chunkSize = maxSize_;
		while (chunks_[i].available > size && i < (numBlocks_ >> 1))
		{
			chunkSize >>= 1;
			size_t child_i = (i << 1) + 1;
			if (chunks_[child_i].available > size) {
				i = child_i;
			}
			else if (chunks_[++child_i].available > size) 
			{
				i = child_i;
				data += chunkSize;
			}
			else 
			{
				chunkSize <<= 1;
				break;
			}
		}

		// mark chunk as allocated
		chunks_[i].allocated = true;
		chunks_[i].available = 0;
		update_available(i, chunkSize);
		
		/* Memory debug
		for (char* c = data; c != data + chunkSize; ++c)
		{
			assert(*c == 'f');
			*c = 'a';
		}
		*/

		return data;
	}

	/** Deallocate memory chunk.
	 * Performs for O(log(maxSize) - log(minSize)).
	 */
	void deallocate(void* mem)
	{
		if (chunks_ == NULL)
		{
			assert(false && "Trying to deallocate after allocator destruction");
			return;
		}

		char* cmem = (char*)mem;
		assert( cmem >= data_ && cmem < data_ + maxSize_ );

		// find allocated chunk
		size_t i = 0;
		char* data = data_;
		size_t chunkSize = maxSize_;
		while (!chunks_[i].allocated || data != mem)
		{
			chunkSize >>= 1;
			size_t child_i = (i << 1) + 1;
			if (cmem < (data + chunkSize)) {
				i = child_i;
			}
			else
			{
				i = child_i + 1;
				data += chunkSize;
			}
		}

		/* Memory debug
		for (char* c = cmem; c != cmem + chunkSize; ++c)
		{
			assert(*c == 'a');
			*c = 'f';
		}
		*/

		// merge free blocks
		chunks_[i].allocated = false;
		chunks_[i].available = chunkSize;
		update_available(i, chunkSize);
	}
	
	/** Get size of the minimum allocatable block */
	size_t min_size() const { return minSize_; }

	/** Get size of the macimum allocatable block, also this is size of storage */
	size_t max_size() const { return maxSize_; }

private:
	size_t get_sibling(size_t index) { return (index & 1) ? (index + 1) : (index - 1); }

	void update_available(size_t i, size_t size)
	{
		size_t maxAvailable = chunks_[i].available;
		while (i > 0)
		{
			chunks_[i].available = maxAvailable;

			// calc maximum available block for parent
			size_t sibling = get_sibling(i);
			if (chunks_[i].available == size && chunks_[sibling].available == size) {
				maxAvailable = (size << 1); // merge available mem
			}
			else {
				maxAvailable = (std::max)(chunks_[sibling].available, chunks_[i].available);
			}
			
			i = (i - 1) >> 1;
			size <<= 1;
		} 
		chunks_[i].available = maxAvailable;
	}

public:
	size_t	minSize_;
	size_t	maxSize_;
	size_t	numBlocks_;
	char*	data_;
	chunk*	chunks_;
};
