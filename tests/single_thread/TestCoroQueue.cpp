
#include <boost/test/unit_test.hpp>
#include "CoroQueue.h"
#include "CoroPool.h"
#include <map>


BOOST_AUTO_TEST_SUITE(SuiteCoroQueue)


BOOST_AUTO_TEST_CASE(TestOneConsumer) {
	std::vector<uint8_t> actual, expected = {1, 2};

	CoroQueue<uint8_t> queue;

	Exec([&]() {
		actual.push_back(queue.pop());
		actual.push_back(queue.pop());
	});
	Exec([&]() {
		queue.push(1);
		queue.push(2);
	});
	Join();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
