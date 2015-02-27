
#include <boost/test/unit_test.hpp>
#include "WsPayloadLength.h"


BOOST_AUTO_TEST_SUITE(SuiteWsPayloadLength)


BOOST_AUTO_TEST_CASE(TestRead) {
	Buffer buffer {0x7e, 0x08, 0x40, 0x8b, 0x00, 0x0b};
	auto it = buffer.begin();
	size_t integer;
	BOOST_REQUIRE_NO_THROW(it = ReadWsPayloadLength(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 2112);
	BOOST_REQUIRE_NO_THROW(it = ReadWsPayloadLength(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 11);
	BOOST_REQUIRE_NO_THROW(it = ReadWsPayloadLength(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 0);
	BOOST_REQUIRE_NO_THROW(it = ReadWsPayloadLength(it, buffer.end(), &integer));
	BOOST_REQUIRE(integer == 11);
}


BOOST_AUTO_TEST_CASE(TestWrite) {
	Buffer buffer(1000);
	WriteWsPayloadLength(buffer, 2112);
	BOOST_REQUIRE(buffer == Buffer({0x7e, 0x08, 0x40}));
	WriteWsPayloadLength(buffer, 0);
	BOOST_REQUIRE(buffer == Buffer({0x00, 0x7e, 0x08, 0x40}));
	WriteWsPayloadLength(buffer, 11);
	BOOST_REQUIRE(buffer == Buffer({0x0b, 0x00, 0x7e, 0x08, 0x40}));
}


BOOST_AUTO_TEST_SUITE_END()
