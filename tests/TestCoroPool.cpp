
#include <boost/test/unit_test.hpp>
#include "coro/CoroPool.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroPool)


BOOST_AUTO_TEST_CASE(TestWaitAll) {
	std::vector<uint8_t> result;

	CoroPool pool;

	pool.exec([&]() {
		result.push_back(1);
	});
	pool.exec([&]() {
		result.push_back(2);
	});

	pool.waitAll();

	BOOST_REQUIRE(result.size() == 2);
}


BOOST_AUTO_TEST_CASE(TestKillAll) {
	CoroPool pool;

	pool.exec([] {
		Coro::current()->yield();
	});
	pool.exec([] {
		Coro::current()->yield();
	});

	pool.killAll();
}


BOOST_AUTO_TEST_SUITE_END()
