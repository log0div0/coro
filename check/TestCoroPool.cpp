
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> actual, expected = {1, 2};

	CoroPool pool;

	pool.fork([&]() {
		actual.push_back(1);
	});
	pool.fork([&]() {
		actual.push_back(2);
	});

	pool.join();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
