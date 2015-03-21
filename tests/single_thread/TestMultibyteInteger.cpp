
#include <boost/test/unit_test.hpp>
#include "MultibyteInteger.h"


BOOST_AUTO_TEST_SUITE(SuiteMultibyteInteger)


BOOST_AUTO_TEST_CASE(TestReadMultibyteInteger) {
	Buffer buffer {0x81, 0x20, 0x00, 0x2b};
	auto it = buffer.begin();
	uint64_t integer;
	BOOST_REQUIRE_NO_THROW(it = ReadMultibyteInteger(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 160);
	BOOST_REQUIRE_NO_THROW(it = ReadMultibyteInteger(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 0);
	BOOST_REQUIRE_NO_THROW(it = ReadMultibyteInteger(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 43);
}


BOOST_AUTO_TEST_CASE(TestWriteMultibyteInteger) {
	Buffer buffer;
	WriteMultibyteInteger(buffer, 43);
	BOOST_REQUIRE(buffer == Buffer({0x2b}));
	WriteMultibyteInteger(buffer, 0);
	BOOST_REQUIRE(buffer == Buffer({0x00, 0x2b}));
	WriteMultibyteInteger(buffer, 160);
	BOOST_REQUIRE(buffer == Buffer({0x81, 0x20, 0x00, 0x2b}));
}


BOOST_AUTO_TEST_SUITE_END()
