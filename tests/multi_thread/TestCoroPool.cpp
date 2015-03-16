
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"
#include "ThreadPool.h"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(TestEmptyCoro) {
	const auto iterations = 10000;
	try {
		for (auto i = 0; i < iterations; i++) {
			cout << "SuiteCoroPool/TestEmptyCoro " << i << " of " << iterations << endl;
			CoroPool pool;
			for (auto j = 0; j < 1024; ++j) {
				pool.exec([&]() { });
			}
		}
	}
	catch (...) {
		BOOST_FAIL("Unexpected exception");
	}
}


BOOST_AUTO_TEST_CASE(TestYieldResume) {
	const auto iterations = 10000;
	try {
		for (auto i = 0; i < iterations; i++) {
			cout << "SuiteCoroPool/TestYieldResume " << i << " of " << iterations << endl;
			CoroPool pool;
			for (auto j = 0; j < 1024; ++j) {
				pool.exec([&]() {
					ThreadPool* threadPool = ThreadPool::current();
					Coro* coro = Coro::current();
					for (auto i = 0; i < 10; ++i) {
						coro->yield([=]() {
							threadPool->schedule([=]() {
								coro->resume();
							});
						});
					}
				});
			}
		}
	}
	catch (...) {
		BOOST_FAIL("Unexpected exception");
	}
}


BOOST_AUTO_TEST_SUITE_END()
