
#include <boost/test/unit_test.hpp>
#include "coro/ObjectFactory.h"


BOOST_AUTO_TEST_SUITE(SuiteObjectFactory)


BOOST_AUTO_TEST_CASE(Test) {
	{
		auto a = ObjectFactory<uint8_t>::make();
		*a = 72;
	}
	{
		auto a = ObjectFactory<uint8_t>::make();
		BOOST_REQUIRE(*a == 72);
	}
}


BOOST_AUTO_TEST_SUITE_END()
