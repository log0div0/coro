
#include <boost/test/unit_test.hpp>
#include "Buffer.h"
#include <algorithm>


BOOST_AUTO_TEST_SUITE(SuiteBuffer)


BOOST_AUTO_TEST_CASE(TestIteratorIncrement) {
	Buffer buffer { 0x01, 0x02, 0x03, 0x04 };

	auto it = buffer.begin();
	BOOST_REQUIRE(*it == 0x01);
	BOOST_REQUIRE(*++it == 0x02);
}


BOOST_AUTO_TEST_CASE(TestInitializerList) {
	Buffer buffer { 0x01, 0x02, 0x03, 0x04 };

	BOOST_REQUIRE(buffer.usefulDataSize() == 4);
	uint8_t temp[] = { 0x01, 0x02, 0x03, 0x04 };
	BOOST_REQUIRE(std::equal(buffer.begin(), buffer.end(), temp));
}

BOOST_AUTO_TEST_CASE(TestPushPop) {
	Buffer buffer(4);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 0);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 4);
	BOOST_REQUIRE(buffer.usefulDataSize() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 4);

	buffer.pushBack(2);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 2);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 2);
	BOOST_REQUIRE(buffer.usefulDataSize() == 2);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 2);

	buffer.popFront(2);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 0);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 4);

	buffer.pushBack(4);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 4);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 0);

	buffer.popFront(3);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 3);

	buffer.pushFront(2);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 3);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 1);
}

BOOST_AUTO_TEST_CASE(TestIteratorDifference) {
	Buffer buffer("abcd");

	auto it = ++buffer.begin();

	BOOST_REQUIRE( (it - buffer.begin()) == 1 );
	BOOST_REQUIRE( (buffer.end() - it) == 3 );
	BOOST_REQUIRE( (buffer.end() - buffer.begin()) == 4 );
}

BOOST_AUTO_TEST_CASE(TestPushFront) {
	std::string str("abcd");

	Buffer buffer(5);
	buffer.pushFront(str.begin(), str.end());
	BOOST_REQUIRE(buffer.usefulDataSize() == 4);
	BOOST_REQUIRE(std::equal(buffer.begin(), buffer.end(), str.begin()));

	BOOST_REQUIRE_NO_THROW(buffer.pushFront(str.begin(), str.end()));
	BOOST_REQUIRE(buffer.size() == 10);
	BOOST_REQUIRE(buffer == Buffer("abcdabcd"));
}

BOOST_AUTO_TEST_CASE(TestPushBack) {
	std::string str("abcd");

	Buffer buffer(5);
	buffer.pushBack(str.begin(), str.end());
	BOOST_REQUIRE(buffer.usefulDataSize() == 4);
	BOOST_REQUIRE(std::equal(buffer.begin(), buffer.end(), str.begin()));

	BOOST_REQUIRE_NO_THROW(buffer.pushBack(str.begin(), str.end()));
	BOOST_REQUIRE(buffer.size() == 10);
	BOOST_REQUIRE(buffer == Buffer("abcdabcd"));
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
}

BOOST_AUTO_TEST_CASE(TestEmpty) {
	printf("0000\n");
	Buffer buffer;
	printf("1111\n");
	BOOST_REQUIRE(buffer.begin() == buffer.end());
	printf("2222\n");
	BOOST_REQUIRE((buffer.end() - buffer.begin()) == 0);
	printf("3333\n");
}

BOOST_AUTO_TEST_SUITE_END()
