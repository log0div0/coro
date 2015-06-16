
#include <boost/test/unit_test.hpp>
#include "coro/Mutex.h"


BOOST_AUTO_TEST_SUITE(SuiteMutex)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> actual, expected = {0, 1, 2, 3};

	Mutex mutex;

	Coro coro1([&] {
		std::lock_guard<Mutex> lock(mutex);
		actual.push_back(0);
		Coro::current()->yield();
		actual.push_back(2);
	});
	Coro coro2([&] {
		actual.push_back(1);
		std::lock_guard<Mutex> lock(mutex);
		actual.push_back(3);
	});
	coro1.resume();
	coro2.resume();
	coro1.resume();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
