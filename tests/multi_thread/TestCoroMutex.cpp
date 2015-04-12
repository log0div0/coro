
#include <boost/test/unit_test.hpp>
#include "CoroMutex.h"
#include "CoroPool.h"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE(SuiteCoroMutex)


BOOST_AUTO_TEST_CASE(Test) {
	const auto iterations = 100000;

	CoroMutex mutex;
	uint64_t counter = 0;

	for (auto i = 0; i < 10; ++i) {
		Exec([&] {
			for (auto i = 0; i < iterations; ++i) {
				std::lock_guard<CoroMutex> lock(mutex);
				++counter;
			}
		});
	}
	Join();

	BOOST_REQUIRE(counter == iterations * 10);
}


BOOST_AUTO_TEST_SUITE_END()
