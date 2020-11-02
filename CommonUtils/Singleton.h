#pragma once

#include <boost/noncopyable.hpp>

namespace iz3d
{
	template< class T >
	class Singleton : public boost::noncopyable
	{
	protected:
		Singleton(){};
		Singleton( const Singleton& ){};
		virtual ~Singleton(){};
	public:
		static T& instance(){
			
			static T inst;
			return inst;
			
		}

		static T& Get()
		{
			return instance();
		}
	};
}