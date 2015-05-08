
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

BOOST_AUTO_TEST_CASE(TestUsefulData) {
	Buffer buffer(5);
	// |OOOOO|
	auto usefulData = buffer.usefulData();
	BOOST_REQUIRE(usefulData.size() == 0);

	buffer.pushBack(5);
	// |XXXXX|
	usefulData = buffer.usefulData();
	BOOST_REQUIRE(usefulData.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(usefulData[0]) == 5);

	buffer.popFront(4);
	buffer.pushBack(2);
	// XX|OO|X
	usefulData = buffer.usefulData();
	BOOST_REQUIRE(usefulData.size() == 2);
	BOOST_REQUIRE(boost::asio::buffer_size(usefulData[0]) == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(usefulData[1]) == 2);

	buffer.popFront(1);
	// |XX|OOO
	usefulData = buffer.usefulData();
	BOOST_REQUIRE(usefulData.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(usefulData[0]) == 2);

	buffer.pushBack(3);
	buffer.popFront(2);
	// OO|XXX|
	usefulData = buffer.usefulData();
	BOOST_REQUIRE(usefulData.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(usefulData[0]) == 3);
}

BOOST_AUTO_TEST_CASE(TestFreeSpace) {
	Buffer buffer(5);
	// |OOOOO|
	auto freeSpace = buffer.freeSpace();
	BOOST_REQUIRE(freeSpace.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(freeSpace[0]) == 5);

	buffer.pushBack(5);
	// |XXXXX|
	freeSpace = buffer.freeSpace();
	BOOST_REQUIRE(freeSpace.size() == 0);

	buffer.popFront(2);
	buffer.popBack(1);
	// OO|XX|O
	freeSpace = buffer.freeSpace();
	BOOST_REQUIRE(freeSpace.size() == 2);
	BOOST_REQUIRE(boost::asio::buffer_size(freeSpace[0]) == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(freeSpace[1]) == 2);

	buffer.pushBack(1);
	// OO|XXX|
	freeSpace = buffer.freeSpace();
	BOOST_REQUIRE(freeSpace.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(freeSpace[0]) == 2);

	buffer.popBack(2);
	buffer.pushFront(2);
	// |XXX|OO
	freeSpace = buffer.freeSpace();
	BOOST_REQUIRE(freeSpace.size() == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(freeSpace[0]) == 2);
}

BOOST_AUTO_TEST_CASE(TestIsUsefulDataContinuousAndFreeSpaceSizeAt) {
	Buffer buffer(5);
	// |OOOOO|
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 5);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 5);
	buffer.pushBack(4);
	// |XXXX|O
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 1);
	buffer.popFront(4);
	// OOOO|O
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 4);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 1);
	buffer.pushBack(4);
	// XXX|O|X
	BOOST_REQUIRE(!buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 0);
	buffer.popFront(1);
	// |XXX|OO
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 2);
	buffer.pushBack(2);
	// |XXXXX|
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 0);
	buffer.popFront(4);
	// OOOO|X|
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 4);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 0);
	buffer.clear(1);
	// O|OOOO
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 1);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 4);
	buffer.clear(-1);
	// OOOO|O
	BOOST_REQUIRE(buffer.isUsefulDataContinuous());
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheBegining() == 4);
	BOOST_REQUIRE(buffer.freeSpaceSizeAtTheEnd() == 1);
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
