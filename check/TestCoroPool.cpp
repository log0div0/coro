
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> actual, expected = {1, 2};

	CoroPool pool;

	auto first = pool.fork([&]() {
		actual.push_back(1);
	});
	auto second = pool.fork([&]() {
		actual.push_back(2);
	});

	BOOST_REQUIRE(pool.wait() == first);
	BOOST_REQUIRE(pool.wait() == second);

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
