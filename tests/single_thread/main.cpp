
#define BOOST_TEST_MODULE test_single_thread
#include <boost/test/unit_test.hpp>
#include "Coro.h"
#include "IoService.h"

int BOOST_TEST_CALL_DECL main(int argc, char* argv[])
{
	int result;

	Coro coro([&] {
		result = boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
	});

	IoService ioService;
	ioService.post([&] {
		coro.resume();
	});
	ioService.run();

	return result;
}