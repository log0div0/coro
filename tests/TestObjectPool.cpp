
#include <boost/test/unit_test.hpp>
#include "ObjectPool.h"


BOOST_AUTO_TEST_SUITE(SuiteObjectPool)


BOOST_AUTO_TEST_CASE(Test) {
	{
		auto a = ObjectPool<uint8_t>::take();
		*a = 72;
	}
	{
		auto a = ObjectPool<uint8_t>::take();
		BOOST_REQUIRE(*a == 72);
	}
}


BOOST_AUTO_TEST_SUITE_END()
