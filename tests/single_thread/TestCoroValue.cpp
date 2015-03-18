
#include <boost/test/unit_test.hpp>
#include "CoroValue.h"
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroValue)


BOOST_AUTO_TEST_CASE(Test1) {
	std::vector<uint8_t> actual, expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};

	CoroValue<uint64_t> value;

	Exec([&]() {
		actual.push_back(1);

		BOOST_REQUIRE(*value == 1111);
		actual.push_back(3);
		BOOST_REQUIRE(*value == 1111);
		actual.push_back(4);
		value.reset();
		actual.push_back(5);

		BOOST_REQUIRE(*value == 2222);
		actual.push_back(7);
		value.reset();
		actual.push_back(8);
	});
	Exec([&]() {
		actual.push_back(2);

		value.set(1111);
		actual.push_back(6);

		value.set(2222);
		actual.push_back(9);
	});
	Join();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
