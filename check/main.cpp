
#define BOOST_TEST_MODULE coro_test
#include <boost/test/unit_test.hpp>
#include "ThreadPool.h"
#include "Coro.h"

ThreadPool g_threadPool;

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	int result;

	Coro coro([&] {
		result = ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	});
	ThreadPool threadPool; // this thread
	threadPool.schedule([&]() {
		coro.resume();
	});
	threadPool.run();

	return result;
}