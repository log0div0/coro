
#include <boost/test/unit_test.hpp>
#include "Buffer.h"
#include <algorithm>


BOOST_AUTO_TEST_SUITE(SuiteBuffer)


BOOST_AUTO_TEST_CASE(TestIteratorIncrement) {
	Buffer buffer { 0x01, 0x02 };

	auto it = buffer.begin();
	BOOST_REQUIRE(*it == 0x01);
	BOOST_REQUIRE(*++it == 0x02);
	BOOST_REQUIRE(++it == buffer.end());
	BOOST_REQUIRE_THROW(*it, std::exception);
	BOOST_REQUIRE_THROW(++it, std::exception);
}

BOOST_AUTO_TEST_CASE(TestInitializerList) {
	Buffer buffer { 0x01, 0x02, 0x03, 0x04 };

	BOOST_REQUIRE(buffer.usefulDataSize() == 4);
	std::vector<uint8_t> temp { 0x01, 0x02, 0x03, 0x04 };
	BOOST_REQUIRE(std::equal(temp.begin(), temp.end(), buffer.begin()));
}

BOOST_AUTO_TEST_CASE(TestIteratorDifference) {
	Buffer buffer("abcd");

	auto it = ++buffer.begin();

	BOOST_REQUIRE( (it - buffer.begin()) == 1 );
	BOOST_REQUIRE( (buffer.end() - it) == 3 );
	BOOST_REQUIRE( (buffer.end() - buffer.begin()) == 4 );
	BOOST_REQUIRE( (buffer.end() - buffer.end()) == 0 );
}

BOOST_AUTO_TEST_CASE(TestPushFrontBack) {
	Buffer buffer(5);
	std::string str("abcd");
	std::string str2("1234");
	std::string str3("5678");

	buffer.assign(str.begin(), str.end());
	BOOST_REQUIRE(buffer.size() == 5);
	BOOST_REQUIRE(buffer.usefulDataSize() == 4);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 1);
	BOOST_REQUIRE(buffer == Buffer("abcd"));

	buffer.pushFront(str2.begin(), str2.end());
	BOOST_REQUIRE(buffer.size() == 10);
	BOOST_REQUIRE(buffer.usefulDataSize() == 8);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 2);
	BOOST_REQUIRE(buffer == Buffer("1234abcd"));

	buffer.pushBack(str3.begin(), str3.end());
	BOOST_REQUIRE(buffer.size() == 20);
	BOOST_REQUIRE(buffer.usefulDataSize() == 12);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 8);
	BOOST_REQUIRE(buffer == Buffer("1234abcd5678"));
}

BOOST_AUTO_TEST_CASE(TestString) {
	Buffer buffer("abcd");
	std::string str(buffer.begin(), buffer.end());
	BOOST_REQUIRE(str == "abcd");
}

BOOST_AUTO_TEST_CASE(TestIteratorMovement) {
	Buffer buffer("abcd");
	auto it = buffer.begin();
	BOOST_REQUIRE(*(it + 2) == 'c');
	BOOST_REQUIRE((it + 4) == buffer.end());
	BOOST_REQUIRE_THROW(it + 5, std::exception);
}

BOOST_AUTO_TEST_CASE(TestEmpty) {
	Buffer buffer;
	BOOST_REQUIRE(buffer.begin() == buffer.end());
	BOOST_REQUIRE((buffer.end() - buffer.begin()) == 0);
	BOOST_REQUIRE(buffer == Buffer());
}


BOOST_AUTO_TEST_SUITE_END()
