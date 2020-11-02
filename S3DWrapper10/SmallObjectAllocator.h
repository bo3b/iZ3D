#pragma once

#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <limits>
#include <memory>
#include <vector>

// Fuck WinAPI
#pragma push_macro("max")
#pragma push_macro("min")
#undef min
#undef max

/** Allocates identical blocks of memory of specified size.
 * WARNING: This allocator has state, you need allocate memory and deallocate using single allocator instance.
 * It is not copyable, though.
 * @tparam ByteAllocator - allocator for allocating bytes, must support allocate(size_t nbytes, void* hint), deallocate(void* ptr, size_t size) functions
 */
template<typename ByteAllocator>
class block_allocator :
	public ByteAllocator,
	public boost::noncopyable
{
BOOST_STATIC_ASSERT( sizeof(ByteAllocator::value_type) == 1 );
public:
	typedef void*			pointer;
	typedef size_t			size_type;
	typedef block_allocator	this_type;
	typedef ByteAllocator	policy_type;
	
private:
	struct chunk;
	typedef std::shared_ptr<chunk> chunk_ptr;

	struct chunk
	{
		policy_type*	alloc_;
		size_t			size_;
		pointer			data_;
		chunk_ptr		prev_;

		chunk(policy_type*	alloc,
			  size_type		size,
              chunk_ptr		prev)
        :   alloc_(alloc)
		,	size_(size)
        ,   prev_(prev)
        {
			data_ = alloc->allocate(size_);
		}

		~chunk()
		{
			if (data_) {
				alloc_->deallocate( (typename policy_type::pointer)data_, size_ );
			}
		}
    };
	
private:
	// grow by 50% or at least to size, same as in std::vector
    size_type grow_to(size_type size)
    {
		return (max_size() - size) < (size >> 1) ? max_size() : (size + (size >> 1));
    }

	pointer& get_node_ptr(pointer ptr)
	{
		return *reinterpret_cast<pointer*>(ptr);
	}

	void link_nodes(char* begin, char* end)
	{
		if (begin != end)
		{
			// each node points to next node
			endNode_ = begin;
			while (begin != end)
			{
				get_node_ptr(begin) = begin + blockSize_;
				begin += blockSize_;
			}
			get_node_ptr(end - blockSize_) = 0;
		}
	}

    void extend(size_type size)
    {
		chunk_ptr chunk( new chunk(this, size * blockSize_, endChunk_) );
        endChunk_ = chunk;
		
		if (endNode_) {
			get_node_ptr(endNode_) = endChunk_->data_;
		}
        link_nodes( (char*)endChunk_->data_, 
				    (char*)endChunk_->data_ + endChunk_->size_ );
    }

	// allocators are always different
	bool operator == (const this_type& other);
	bool operator != (const this_type& other);

public:
    block_allocator( size_type			blockSize, 
					 size_type			capacity = 0,
					 const policy_type& policy = policy_type() )
    :   policy_type(policy)
	,	blockSize_(blockSize)
	,	capacity_(0)
    ,   endNode_(0)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numAllocations_(0)
	,	numDeallocations_(0)
	,	maxNumActiveAllocations_(0)
#endif
    {
		assert( blockSize_ >= sizeof(pointer) );
		reserve(capacity);
    }

    void reserve(size_type newCapacity)
    {
        if (capacity_ >= newCapacity) {
            return;
        }

        extend(newCapacity - capacity_);
        capacity_ = newCapacity;
    }

    void clear()
    {
        endChunk_.reset();
    }

    void deallocate(pointer node)
    {
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numDeallocations_;
#endif
		get_node_ptr(node) = endNode_;
		endNode_ = node;
    }

	pointer allocate()
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numAllocations_;
		maxNumActiveAllocations_ = std::max(numAllocations_ - numDeallocations_, maxNumActiveAllocations_);
#endif
		if (!endNode_) {
			reserve( grow_to(capacity_ + 1) );
		}

		pointer allocated = endNode_;
		endNode_ = get_node_ptr(endNode_); 
		return allocated;
	}

    bool      empty() const		 { return endNode_ == 0; }
	size_type block_size() const { return blockSize_; }
    size_type capacity() const	 { return capacity_; }
	
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	num_allocations() const				{ return numAllocations_; }
	size_type	num_deallocations() const			{ return numDeallocations_; }
	size_type	max_num_active_allocations() const	{ return maxNumActiveAllocations_; }
#endif

private:
	size_type	blockSize_;
    size_type	capacity_;
    chunk_ptr	endChunk_;
	pointer		endNode_;
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	numAllocations_;
	size_type	numDeallocations_;
	size_type	maxNumActiveAllocations_;
#endif
};

/// structure to specify allocator distribution
struct allocation_desc
{
	size_t	size;	///	size of the block
	size_t	count;	/// number of initially allocated elements
};

/** Allocator based on google TCMalloc approach for small objects: 
 * http://goog-perftools.sourceforge.net/doc/tcmalloc.html.
 * Also may also find usefull resources on Intel TBB: 
 * http://download.intel.com/technology/itj/2007/v11i4/5-foundations/5-Foundations_for_Scalable_Multi-core_Software.pdf
 * WARNING: This allocator has state - pointers to the block allocators, they are copied in copy constructor.
 * NOTICE: Can be optimized by removing indirect usage of block allocators. But need to be extra carefull, because
 * standart rely on assumption that allocators don't have state.
 * @tparam Policy - allocator for allocating large chunks of data, that will be distributed along small allocations.
 */
template< typename T,
		  typename Policy = std::allocator<T> >
class small_object_allocator :
	public Policy
{
template<typename U, typename P>
friend class small_object_allocator;
public:
	typedef Policy					policy_type;
	typedef small_object_allocator	this_type;

	typedef typename Policy::value_type			value_type;
	typedef typename Policy::pointer			pointer;
	typedef typename Policy::reference			reference;
	typedef typename Policy::const_pointer		const_pointer;
	typedef typename Policy::const_reference	const_reference;
	typedef typename Policy::size_type			size_type;
	typedef typename Policy::difference_type	difference_type;

    // convert an allocator<T> to allocator<U>
    template<typename U>
    struct rebind {
        typedef small_object_allocator<U, typename Policy::rebind<U>::other> other;
    };

public:	
	static const size_type		 base_num_classes = 18;				// number of allocator classes
	static const allocation_desc base_classes[base_num_classes];	// allocator classes(sizes of blocks)

private:
	typedef typename policy_type::rebind<char>::other	byte_allocator_type;
	typedef block_allocator<byte_allocator_type>		block_allocator_type;
	typedef boost::shared_array<block_allocator_type>	block_allocator_array;

	typedef typename policy_type::rebind<block_allocator_type>::other	block_allocator_allocator;

	struct allocator_deallocator
	{
		allocator_deallocator(block_allocator_allocator& allocator, size_t numAllocators)
		:	allocator_(allocator)
		,	numAllocators_(numAllocators)
		{
		}

		void operator () (block_allocator_type* ptr)
		{
			for (auto pA = ptr; pA != ptr + numAllocators_; ++pA) {
				allocator_.destroy(pA);
			}
			allocator_.deallocate(ptr, numAllocators_);
		}

		block_allocator_allocator& allocator_;
		size_t numAllocators_;
	};

	
	template<typename Iterator>
	void setup_allocators(Iterator begin, Iterator end)
	{
		numAllocators_ = std::distance(begin, end);
		allocators_.reset( blockAllocatorAllocator_.allocate(numAllocators_), 
						   allocator_deallocator(blockAllocatorAllocator_, numAllocators_) );

		block_allocator_type* pA = allocators_.get();
		for (Iterator it = begin; it != end; ++it, ++pA) {
			new(pA) block_allocator_type(it->size * sizeof(T), it->count, *this);
		}
	}

private:
	// better don't mess with copy
	this_type& operator = (const this_type& other);

	block_allocator_type* begin_allocator() { return allocators_.get(); }
	block_allocator_type* end_allocator()   { return allocators_.get() + numAllocators_; }

public:
	small_object_allocator()
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	:	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
		setup_allocators(base_classes, base_classes + base_num_classes);
	}
	
	/** Create allocator with custom block size distribution. 
	 * @tparam Iterator - iterator addressing allocation descriptions (block_size, capacity) for block allocators. Range must be ordered in
	 * ascending order by block_size.
	 * @see allocation_desc
	 */
	template<typename Iterator>
	small_object_allocator(Iterator begin, Iterator end)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	:	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
		setup_allocators(begin, end);
	}

	small_object_allocator(const small_object_allocator& other)
	:	policy_type(other)
	,	numAllocators_(other.numAllocators_)
	,	allocators_(other.allocators_)
	,	blockAllocatorAllocator_(other.blockAllocatorAllocator_)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
	}

	~small_object_allocator()
	{
		// for debug convenience
	}

	template<typename U, typename P>
	explicit small_object_allocator(const small_object_allocator<U, P>& other)
	:	policy_type(other)
	,	numAllocators_(other.numAllocators_)
	,	allocators_(other.allocators_)
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	,	numExcessAllocations_(0)
	,	minAllocationSize_( std::numeric_limits<size_type>::max() )
	,	maxAllocationSize_(0)
#endif 
	{
	}

	pointer allocate(size_type count, const_pointer hint = 0)
	{
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		minAllocationSize_ = std::min(minAllocationSize_, count);
		maxAllocationSize_ = std::max(maxAllocationSize_, count);
#endif 
		// find allocator class
		block_allocator_type* iter = std::lower_bound( begin_allocator(),
													   end_allocator(),
													   count * sizeof(value_type),
													   [] (const block_allocator_type& a, size_type s) { return a.block_size() < s; } );

		if ( iter != end_allocator() ) {
			return (pointer)iter->allocate(); // allocate block of sufficient size
		}

#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
		++numExcessAllocations_;
#endif
		// allocate using default allocation policy
		return policy_type::allocate(count, hint);
	}

	void deallocate(pointer ptr, size_type count)
	{
		block_allocator_type* iter = std::lower_bound( begin_allocator(),
													   end_allocator(),
													   count * sizeof(value_type),
													   [] (const block_allocator_type& a, size_type s) { return a.block_size() < s; } );
		
		if ( iter != end_allocator() ) {
			iter->deallocate(ptr);
		}
		else 
		{
			// deallocate using default allocation policy
			policy_type::deallocate(ptr, count);
		}
	}

	bool operator == (const this_type& other)
	{
		return (numAllocators_ == other.numAllocators_) 
			   && (blockAllocatorAllocator_ == other.blockAllocatorAllocator_)
			   && (allocators_ == other.allocators_);
	}

	bool operator != (const this_type& other)
	{
		return !(*this == other);
	}

	size_t num_block_allocators() const
	{
		return numAllocators_;
	}
	
	block_allocator_type& get_block_allocator(size_t i)
	{
		assert(i < numAllocators_);
		return allocators_[i];
	}

	const block_allocator_type& get_block_allocator(size_t i) const
	{
		assert(i < numAllocators_);
		return allocators_[i];
	}
	
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type num_excess_allocations() const	{ return numExcessAllocations_; }
	size_type min_allocation() const			{ return minAllocationSize_ == std::numeric_limits<size_type>::max() ? 0 : minAllocationSize_; }
	size_type max_allocation() const			{ return maxAllocationSize_; }
#endif

private:
	size_t						numAllocators_;
	block_allocator_array		allocators_;
	block_allocator_allocator	blockAllocatorAllocator_;

public:
#ifdef SMALL_OBJECT_ALLOCATOR_STATISTICS
	size_type	numExcessAllocations_;
	size_type	maxAllocationSize_;
	size_type	minAllocationSize_;
#endif
};

// default allocator classes (block size, capacity)
template<typename T, typename P>
const allocation_desc small_object_allocator<T, P>::base_classes[small_object_allocator<T, P>::base_num_classes] =
{
	{4,   16},
	{8,   16},
	{12,  8},
	{16,  8},
	{24,  8},
	{32,  4},
	{40,  4},
	{48,  4},
	{56,  2},
	{64,  2},
	{80,  2},
	{96,  2},
	{112, 1},
	{128, 1},
	{160, 1},
	{192, 1},
	{224, 1},
	{256, 1}
};

#pragma pop_macro("min")
#pragma pop_macro("max")