#pragma once

#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

namespace iz3d
{
	template< class T >
	class PerThreadSingleton : public boost::noncopyable
	{
	protected:
		PerThreadSingleton(){};
		PerThreadSingleton( const PerThreadSingleton& ){};

	public:
		virtual ~PerThreadSingleton(){};

		static T& instance()
		{
			static boost::thread_specific_ptr<T> ptr;
			T* t = ptr.get();
			if (!t)
			{
				t = new T();
				ptr.reset(t);
			}
			return *t;
		}

		static T& Get()
		{
			return instance();
		}
	};
}