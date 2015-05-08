
#include <boost/test/unit_test.hpp>
#include "UdpSocket.h"
#include "CoroPool.h"


using namespace boost::asio::ip;


BOOST_AUTO_TEST_SUITE(SuiteUdp)


BOOST_AUTO_TEST_CASE(TestSendReceive) {
	udp::endpoint serverEndpoint(address_v4::from_string("127.0.0.1"), 44442), senderEndpoint;
	Buffer testData { 0x01, 0x02, 0x03, 0x04 };

	UdpSocket server(serverEndpoint);
	UdpSocket client;

	client.send(testData, serverEndpoint);

	{
		Buffer tempData;
		tempData.pushBack(server.receive(&tempData, &senderEndpoint));
		BOOST_REQUIRE(tempData == testData);
	}

	server.send(testData, senderEndpoint);

	{
		Buffer tempData(10000);
		tempData.pushBack(client.receive(&tempData, &senderEndpoint));
		BOOST_REQUIRE(tempData == testData);
		BOOST_REQUIRE(serverEndpoint == senderEndpoint);
	}
}


BOOST_AUTO_TEST_SUITE_END()
