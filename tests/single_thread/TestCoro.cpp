
#include <boost/test/unit_test.hpp>
#include "Coro.h"


BOOST_AUTO_TEST_SUITE(SuiteCoro)


BOOST_AUTO_TEST_CASE(Test) {
	auto coro1 = Coro::current();
	Coro coro2(
		[&]() {},
		[&]() {
			coro1->resume();
		}
	);
	coro1->yield([&]() {
		coro2.resume();
	});
}


BOOST_AUTO_TEST_SUITE_END()
