
#include <boost/test/unit_test.hpp>
#include "CoroUtils.h"


BOOST_AUTO_TEST_SUITE(SuiteCoroUtils)


BOOST_AUTO_TEST_CASE(TestParallel) {
	uint32_t a = 0, b = 0;

	Parallel({
		[&] {
			a = 10;
		},
		[&] {
			b = 20;
		}
	});

	BOOST_REQUIRE(a == 10);
	BOOST_REQUIRE(b == 20);
}


BOOST_AUTO_TEST_SUITE_END()
