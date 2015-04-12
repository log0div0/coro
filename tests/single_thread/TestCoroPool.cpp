
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> result;

	CoroPool pool;

	pool.exec([&]() {
		result.push_back(1);
	});
	pool.exec([&]() {
		result.push_back(2);
	});

	pool.join();

	BOOST_REQUIRE(result.size() == 2);
}


BOOST_AUTO_TEST_SUITE_END()
