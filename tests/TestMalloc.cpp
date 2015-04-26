
#include <boost/test/unit_test.hpp>
#include "Malloc.h"


BOOST_AUTO_TEST_SUITE(SuiteMalloc)


BOOST_AUTO_TEST_CASE(Test) {
	{
		auto a = Malloc<uint8_t>();
		*a = 72;
	}
	{
		auto a = Malloc<uint8_t>();
		BOOST_REQUIRE(*a == 72);
	}
}


BOOST_AUTO_TEST_SUITE_END()
