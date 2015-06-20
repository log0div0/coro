
#include <boost/test/unit_test.hpp>
#include "coro/UdpSocket.h"
#include "coro/CoroPool.h"


using namespace boost::asio::ip;


BOOST_AUTO_TEST_SUITE(SuiteUdp)


BOOST_AUTO_TEST_CASE(TestSendReceive) {
	udp::endpoint serverEndpoint(address_v4::from_string("127.0.0.1"), 44442), senderEndpoint;
	std::vector<uint8_t> testData { 0x01, 0x02, 0x03, 0x04 };

	UdpSocket server(serverEndpoint);
	UdpSocket client;

	client.send(boost::asio::buffer(testData), serverEndpoint);

	{
		std::vector<uint8_t> tempData(4);
		server.receive(boost::asio::buffer(tempData), senderEndpoint);
		BOOST_REQUIRE(tempData == testData);
	}

	server.send(boost::asio::buffer(testData), senderEndpoint);

	{
		std::vector<uint8_t> tempData(4);
		client.receive(boost::asio::buffer(tempData), senderEndpoint);
		BOOST_REQUIRE(tempData == testData);
		BOOST_REQUIRE(serverEndpoint == senderEndpoint);
	}
}


BOOST_AUTO_TEST_SUITE_END()
