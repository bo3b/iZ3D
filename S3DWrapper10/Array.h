#pragma once

#include <boost/type_traits/is_fundamental.hpp>
#include <boost/utility/enable_if.hpp>

namespace {

	// May be replace with some 3rd-party template 
	template<bool B, typename T1, typename T2>
	struct if_then_else
	{
		typedef T1 type;
	};

	template<typename T1, typename T2>
	struct if_then_else<false, T1, T2>
	{
		typedef T2 type;
	};

} // anonymous namespace

template<typename T>
struct array_traits
{
	typedef typename boost::is_fundamental<T>::type no_construct;
	typedef typename boost::is_fundamental<T>::type no_destroy;
};

/** Array with vector like-interface, lightweight iterators and functionality. 
 * Interface is similar to std::vector, except that it doesn't have 'reserve' and 'push_back'.
 */
template< typename T, 
		  typename Allocator = std::allocator<T>,
		  typename Traits = array_traits<T> >
class array
{
public:
	typedef Traits										traits_type;

	typedef Allocator									allocator_type;
	typedef typename allocator_type::size_type			size_type;
	typedef typename allocator_type::difference_type	difference_type;
	typedef typename allocator_type::pointer			pointer;
	typedef typename allocator_type::const_pointer		const_pointer;
	typedef typename allocator_type::reference			reference;
	typedef typename allocator_type::const_reference	const_reference;

	typedef T											value_type;
	typedef T*											iterator;
	typedef const T*									const_iterator;
	typedef std::reverse_iterator<iterator>				reverse_iterator;
	typedef std::reverse_iterator<const_iterator>		const_reverse_iterator;

public:
	// switch between reference or value transfer for better performance, specialize if necessary
	template<typename T>
	struct rparam
	{	
		typedef typename if_then_else<sizeof(T) <= sizeof(long long), const T, const T&>::type type;
	};
	
private:
	template<typename Traits>
	void construct_range(iterator /*begin*/, 
					     iterator /*end*/,
					     typename boost::enable_if<typename Traits::no_construct>::type* dummy = 0)
	{
	}
	
	template<typename Traits>
	void construct_range(iterator begin, 
					     iterator end,
					     typename boost::disable_if<typename Traits::no_construct>::type* dummy = 0)
	{
		value_type value;
		for (; begin != end; ++begin) {
			allocator_.construct(begin, value);
		}
	}
	
	template<typename Traits>
	void destroy_range(iterator /*begin*/, 
					   iterator /*end*/,
					   typename boost::enable_if<typename Traits::no_destroy>::type* dummy = 0)
	{
	}
	
	template<typename Traits>
	void destroy_range(iterator begin, 
					   iterator end,
					   typename boost::disable_if<typename Traits::no_destroy>::type* dummy = 0)
	{
		for (; begin != end; ++begin) {
			allocator_.destroy(begin);
		}
	}
	
	template<typename Iterator>
	array(Iterator	beginIt, 
		  Iterator	endIt,
		  size_type size,
		  typename rparam<allocator_type>::type allocator)
	:	allocator_(allocator)
	,	data_(0)
	,	size_(size)
	{
		assert( size_ >= (size_type)std::distance(beginIt, endIt) );
		if (size_ > 0) 
		{
			data_ = allocator_.allocate(size);
			auto idata = begin();
			for (auto it = beginIt; it != endIt; ++it, ++idata) {
				allocator_.construct(idata, *it);
			}
			if ( idata != end() ) {
				construct_range<traits_type>(idata, end());
			}
		}
	}

public:
	explicit array(typename rparam<allocator_type>::type allocator = allocator_type())
	:	size_(0)
	,	data_(0)
	,	allocator_(allocator)
	{
	}

	explicit array(size_type size)
	:	size_(size)
	,	data_(0)
	{
		if (size_ > 0)
		{
			data_ = allocator_.allocate(size_);
			construct_range<traits_type>(begin(), end());
		}
	}

	array(size_type size,
		  typename rparam<value_type>::type value,
		  typename rparam<allocator_type>::type allocator = allocator_type())
	:	size_(size)
	,	data_(0)
	,	allocator_(allocator)
	{
		if (size_ > 0)
		{
			data_ = allocator_.allocate(size_);
			for (auto it = begin(); it != end(); ++it) {
				allocator_.construct(it, value);
			}
		}
	}

	template<typename Iterator>
	array(Iterator beginIt, 
		  Iterator endIt,
		  typename rparam<allocator_type>::type allocator = allocator_type())
	:	allocator_(allocator)
	,	data_(0)
	{
		size_ = std::distance(beginIt, endIt);
		if (size_ > 0) 
		{
			data_ = allocator_.allocate(size_);
			auto idata = begin();
			for (auto it = beginIt; it != endIt; ++it, ++idata) {
				allocator_.construct(idata, *it);
			}
		}
	}

	array(const array& other)
	:	size_(other.size_)
	,	data_(0)
	,	allocator_(other.allocator_)
	{
		if (size_ > 0)
		{
			data_ = allocator_.allocate(size_);
			auto idata = begin();
			for (auto it = other.begin(); it != other.end(); ++it, ++idata) {
				allocator_.construct(idata, *it);
			}
		}
	}

	~array()
	{
		if (data_)
		{
			destroy_range<traits_type>(begin(), end());
			allocator_.deallocate(data_, size_);
		}
	}

	array& array::operator = (const array& other)
	{       
		if (this != &other) 
		{
			array tmp(other.begin(), other.end(), _allocator); // 'array tmp(other)' is shorter, but better to use
															   // our allocator because of heavy swap in case of different allocators
			swap(tmp);
		}

		return *this;
	}

	void swap(array& other) throw()
	{
		if (this == &other) /* nothing */;
		else if (allocator_ == other.allocator_)
		{
			std::swap(size_, other.size_);
			std::swap(data_, other.data_);
		}
		else 
		{
			// can't assign allocators, so can't use std::swap, so perform per element swap
			array v(other);
			assign( other.begin(), other.end() );
			other.assign( v.begin(), v.end() );
		}
	}

	size_type				size() const			{ return size_; }
	bool					empty() const			{ return size_ == 0; }
	size_type				max_size() const		{ return allocator_.max_size(); }
	const allocator_type&	get_allocator() const	{ return allocator_; }

	reference		front()			{ return *data_; }
	const_reference front() const	{ return *data_; }
	reference		back()			{ return *(end() - 1); }
	const_reference back() const	{ return *(end() - 1); }

	iterator		begin()			{ return data_; }
	iterator		end()			{ return data_ + size_; }
	const_iterator	begin()	const	{ return data_; }
	const_iterator	end()	const	{ return data_ + size_; }

	reverse_iterator		rbegin()			{ return reverse_iterator( begin() ); }
	reverse_iterator		rend()				{ return reverse_iterator( end() ); }
	const_reverse_iterator	rbegin() const		{ return reverse_iterator( begin() ); }
	const_reverse_iterator	rend() const		{ return reverse_iterator( end() ); }

	pointer data()					{ return data_; }
	const_pointer data() const		{ return data_; }

	const_reference operator [] (size_type i) const
	{
		assert(i < size_);
		return (*(data_ + i));
	}

	reference operator [] (size_type i)
	{
		assert(i < size_);
		return (*(data_ + i));
	}
	
	const_reference at(size_type i) const
	{
		if (i >= size_) {
			throw std::out_of_range();
		}
		return (*(data_ + _Pos));
	}

	reference at(size_type i)
	{
		if (i >= size_) {
			throw std::out_of_range();
		}
		return (*(data_ + i));
	}
	
	void resize(size_type size)
	{
		if (size != size_)
		{
			array v(begin(), begin() + (std::min)(size, size_), size, allocator_);
			v.swap(*this);
		}
	}

	void clear()
	{
		if (data_)
		{
			destroy_range<traits_type>(begin(), end());
			allocator_.deallocate(data_, size_);
			data_ = 0;
			size_ = 0;
		}
	}

	template <class Iterator>
	void assign(Iterator first, Iterator last)
	{
		resize( std::distance(first, last) );
		std::copy(first, last, data_);
	}

	void assign(size_type n, typename rparam<value_type>::type u)
	{
		resize(n);
		std::fill(begin(), end(), u);
	}

private:
	pointer			data_;		// pointer to beginning of array
	size_type		size_;		// size of array
	allocator_type	allocator_;	// allocator object for values
};
