#define BOOST_TEST_MODULE first_test_suite
#include <boost/test/unit_test.hpp>

// for Boost
#include <exception>
namespace boost // for shared_ptr
{
 
	void throw_exception( std::exception const & e ) 
	{
	}
}

BOOST_AUTO_TEST_SUITE( test_suite1 )

BOOST_AUTO_TEST_CASE( test1 )
{
	BOOST_CHECK( 2 == 2 );
}

BOOST_AUTO_TEST_CASE( test2 )
{
	BOOST_CHECK( 2 == 2 );
}

BOOST_AUTO_TEST_SUITE_END()