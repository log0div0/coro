
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"
#include "ThreadPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(TestEmptyCoro) {
	try {
		for (auto i = 0; i < 10000; i++) {
			printf("-------- %d --------\n", i);
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
	try {
		for (auto i = 0; i < 10000; i++) {
			printf("-------- %d --------\n", i);
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
