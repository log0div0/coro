
#include <boost/test/unit_test.hpp>
#include "CoroQueue.h"
#include "CoroUtils.h"
#include <map>


BOOST_AUTO_TEST_SUITE(SuiteCoroQueue)


BOOST_AUTO_TEST_CASE(TestOneConsumer) {
	std::vector<uint8_t> actual, expected = {1, 2};

	CoroQueue<uint8_t> queue;

	Parallel({
		[&]() {
			actual.push_back(queue.pop());
			actual.push_back(queue.pop());
		},
		[&]() {
			queue.push(1);
			queue.push(2);
		}
	});

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_CASE(TestTwoConsumers) {
	std::map<uint8_t, uint8_t> actual, expected = {{1, 1}, {2, 2}};

	CoroQueue<uint8_t> queue;

	Parallel({
		[&]() {
			actual[1] = queue.pop();
		},
		[&]() {
			actual[2] = queue.pop();
		},
		[&]() {
			queue.push(1);
			queue.push(2);
		}
	});

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
