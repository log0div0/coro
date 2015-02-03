
#include <boost/test/unit_test.hpp>
#include "ThreadPool.h"


extern ThreadPool g_threadPool;


BOOST_AUTO_TEST_SUITE(SuiteThreadPool)


BOOST_AUTO_TEST_CASE(TestParallel) {
	uint32_t a = 0, b = 0;

	g_threadPool.parallel({
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
