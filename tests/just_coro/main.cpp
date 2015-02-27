
#define BOOST_TEST_MODULE coro_test_coro
#include <boost/test/unit_test.hpp>

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	return ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}