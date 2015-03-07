
#define BOOST_TEST_MODULE test_single_thread
#include <boost/test/unit_test.hpp>
#include "ThreadPool.h"
#include "Coro.h"

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	int result;

	Coro coro([&] {
		result = ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	});
	ThreadPool threadPool(1);
	threadPool.schedule([&]() {
		coro.resume();
	});

	return result;
}