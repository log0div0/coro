
#define BOOST_TEST_MODULE coro_test
#include <boost/test/unit_test.hpp>
#include "ThreadPool.h"

ThreadPool g_threadPool;

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	int result;

	g_threadPool.main([&] {
		result = ::boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
	});

	return result;
}