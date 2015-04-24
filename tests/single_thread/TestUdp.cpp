
#include <boost/test/unit_test.hpp>
#include "UdpSocket.h"
#include <iostream>


using namespace boost::asio::ip;

static auto endpoint = udp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteUdp)


BOOST_AUTO_TEST_CASE(TestSendReceive) {
	UdpSocket server(endpoint);
	UdpSocket client(udp::endpoint(address_v4::from_string("127.0.0.1"), 44443));
	client.send(test_data, endpoint);
	Buffer data;
	udp::endpoint endpoint;
	data.pushBack(server.receive(&data, &endpoint));
	BOOST_REQUIRE(data == test_data);
	BOOST_REQUIRE(endpoint == client.endpoint());
}


BOOST_AUTO_TEST_SUITE_END()
