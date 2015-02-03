
#include <boost/test/unit_test.hpp>
#include "Buffer.h"


BOOST_AUTO_TEST_SUITE(SuiteBuffer)


BOOST_AUTO_TEST_CASE(TestInitializerList) {
	DynamicBuffer buffer { 0x01, 0x02, 0x03, 0x04 };

	BOOST_REQUIRE(buffer.size() == 4);
	uint8_t temp[] = { 0x01, 0x02, 0x03, 0x04 };
	BOOST_REQUIRE(!memcmp(temp, buffer.data(), buffer.size()));
}


BOOST_AUTO_TEST_SUITE_END()
