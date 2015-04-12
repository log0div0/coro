
#include <boost/test/unit_test.hpp>
#include "CoroMutex.h"
#include "CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroMutex)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> actual,
		expected1 = {1, 2, 3, 4, 5, 6, 7, 8},
		expected2 = {1, 2, 4, 5, 3, 6, 7, 8};

	CoroMutex mutex;
	mutex.lock();

	actual.push_back(1);
	Exec([&]() {
		actual.push_back(2);
		std::lock_guard<CoroMutex> lock(mutex);
		actual.push_back(6);
	});
	Exec([&]() {
		actual.push_back(3);
		std::lock_guard<CoroMutex> lock(mutex);
		actual.push_back(7);
	});
	Exec([&]() {
		actual.push_back(4);
		mutex.unlock();
		actual.push_back(5);
	});
	Join();
	actual.push_back(8);

	BOOST_REQUIRE(actual == expected1 || actual == expected2);
}


BOOST_AUTO_TEST_SUITE_END()
