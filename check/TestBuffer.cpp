
#include <boost/test/unit_test.hpp>
#include "Buffer.h"
#include <algorithm>


BOOST_AUTO_TEST_SUITE(SuiteBuffer)


BOOST_AUTO_TEST_CASE(TestIteratorIncrementDecrement) {
	Buffer buffer { 0x01, 0x02, 0x03, 0x04 };

	auto it = buffer.begin();
	BOOST_REQUIRE(*it == 0x01);
	BOOST_REQUIRE(*++it == 0x02);
	BOOST_REQUIRE(*--it == 0x01);
}


BOOST_AUTO_TEST_CASE(TestInitializerList) {
	Buffer buffer { 0x01, 0x02, 0x03, 0x04 };

	BOOST_REQUIRE(buffer.size() == 4);
	uint8_t temp[] = { 0x01, 0x02, 0x03, 0x04 };
	BOOST_REQUIRE(std::equal(buffer.begin(), buffer.end(), temp));
}

BOOST_AUTO_TEST_CASE(TestExpandReduce) {
	Buffer buffer(4);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 0);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 4);
	BOOST_REQUIRE(buffer.usefulDataSize() == 0);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 4);

	buffer.expandBack(2);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 2);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 2);
	BOOST_REQUIRE(buffer.usefulDataSize() == 2);
	BOOST_REQUIRE(buffer.freeSpaceSize() == 2);

	buffer.reduceFront(2);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 0);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 4);

	buffer.expandBack(4);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 4);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 0);

	buffer.reduceFront(3);

	BOOST_REQUIRE(boost::asio::buffer_size(buffer.usefulData()) == 1);
	BOOST_REQUIRE(boost::asio::buffer_size(buffer.freeSpace()) == 3);
}


BOOST_AUTO_TEST_SUITE_END()
