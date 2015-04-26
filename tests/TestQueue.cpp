
#include <boost/test/unit_test.hpp>
#include "Queue.h"


BOOST_AUTO_TEST_SUITE(SuiteQueue)


BOOST_AUTO_TEST_CASE(Test) {
	std::vector<uint8_t> actual, expected = {0, 1, 2, 3};

	Queue<uint8_t> queue;

	Coro consumer([&] {
		for (auto i = 0; i < 4; ++i) {
			actual.push_back(queue.pop());
		}
	});
	Coro producer([&] {
		for (auto i = 0; i < 4; ++i) {
			queue.push(i);
		}
	});
	consumer.resume();
	producer.resume();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_CASE(TestCancel) {
	std::vector<uint8_t> actual, expected = {};

	Queue<uint8_t> queue;

	Coro consumer([&] {
		actual.push_back(queue.pop());
	});
	Coro producer([&] {
		queue.push(0);
	});
	consumer.resume();
	consumer.cancel();
	producer.resume();

	BOOST_REQUIRE(actual == expected);
}


BOOST_AUTO_TEST_SUITE_END()
