
#include <boost/test/unit_test.hpp>
#include "CoroPool.h"
#include <iostream>

using namespace std;

BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(TestExec) {
	const auto iterations = 100000;
	CoroPool pool;
	for (auto i = 0; i < iterations; i++) {
		pool.exec([&]() { });
	}
}


BOOST_AUTO_TEST_CASE(TestJoin) {
	const auto iterations = 100000;
	for (auto i = 0; i < iterations; i++) {
		CoroPool pool;
		pool.exec([&]() { });
	}
}


BOOST_AUTO_TEST_SUITE_END()
