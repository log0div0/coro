
#include <boost/test/unit_test.hpp>
#include "CoroValue.h"
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroValue)


BOOST_AUTO_TEST_CASE(TestSetReset) {
	std::vector<uint8_t> actual, expected = {1, 2, 3, 4, 5, 6, 7, 8, 9};

	CoroValue<uint64_t> value;

	Exec([&]() {
		actual.push_back(1);

		{
			std::lock_guard<CoroValue<uint64_t>> lock(value);
			BOOST_REQUIRE(*value == 1111);
			actual.push_back(3);
			BOOST_REQUIRE(*value == 1111);
			actual.push_back(4);
		}

		actual.push_back(5);

		{
			std::lock_guard<CoroValue<uint64_t>> lock(value);
			BOOST_REQUIRE(*value == 2222);
			actual.push_back(7);
		}

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


BOOST_AUTO_TEST_CASE(TestInvalidate) {
	bool success = false;

	CoroValue<uint64_t> value;
	value.invalidate();

	Exec([&]() {
		try {
			value.wait();
		}
		catch (const InvalidValueError& error) {
			success = true;
		}
	});

	Join();

	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_CASE(TestInvalidate2) {
	bool success = false;

	CoroValue<uint64_t> value;

	Exec([&]() {
		BOOST_REQUIRE_NO_THROW(value.wait());
		value.reset();
		BOOST_REQUIRE_THROW(value.wait(), InvalidValueError);
		success = true;
	});
	Exec([&]() {
		value.set(1111);
		value.invalidate();
	});

	Join();

	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_SUITE_END()
