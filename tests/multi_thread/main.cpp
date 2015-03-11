
#define BOOST_TEST_MODULE test_multi_thread
#include <boost/test/unit_test.hpp>
#include "ThreadPool.h"
#include "Coro.h"
#include <iostream>

int BOOST_TEST_CALL_DECL
main( int argc, char* argv[] )
{
	int result;

	Coro coro([&] {
		result = ::boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	});

	{
		ThreadPool threadPool(std::thread::hardware_concurrency());
		threadPool.schedule([&]() {
			coro.resume();
		});
	}

	std::cout << "OK" << std::endl;

	return result;
}