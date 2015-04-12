
#include <boost/test/unit_test.hpp>
#include "CoroQueue.h"
#include "CoroPool.h"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE(SuiteCoroQueue)


BOOST_AUTO_TEST_CASE(TestOneProducerOneConsumer) {
	const auto iterations = 1000000;
	CoroQueue<uint64_t> queue;
	Exec([&] {
		for (auto i = 0; i < iterations; i++) {
			queue.push(i);
		}
	});
	Exec([&] {
		for (auto i = 0; i < iterations; i++) {
			queue.pop();
		}
	});
	Join();
}


BOOST_AUTO_TEST_CASE(TestSeveralProducersSeveralConsumers) {
	const auto iterations = 100000;
	CoroQueue<uint64_t> queue;
	for (auto i = 0; i < 10; ++i) {
		Exec([&] {
			for (auto i = 0; i < iterations; i++) {
				queue.push(i);
			}
		});
	}
	for (auto i = 0; i < 10; ++i) {
		Exec([&] {
			for (auto i = 0; i < iterations; i++) {
				queue.pop();
			}
		});
	}
	Join();
}


BOOST_AUTO_TEST_SUITE_END()
