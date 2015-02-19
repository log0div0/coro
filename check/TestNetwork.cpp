
#include <boost/test/unit_test.hpp>
#include "TcpServer.h"
#include "NetworkIterator.h"


extern ThreadPool g_threadPool;
auto endpoint = TcpEndpoint(IPv4Address::from_string("127.0.0.1"), 44442);
Buffer test_data({ 0x01, 0x02, 0x03, 0x04 });
Buffer test_data2({ 0x05, 0x06, 0x07, 0x08 });


BOOST_AUTO_TEST_SUITE(SuiteNetwork)


BOOST_AUTO_TEST_CASE(TestTcpSocketAndServer) {
	bool serverDone = false, clientDone = false;

	g_threadPool.parallel({
		[&]() {
			TcpServer server(g_threadPool, endpoint);
			TcpSocket socket = server.accept();
			Buffer data(4);
			socket.receiveData(&data);
			BOOST_REQUIRE(data == test_data);
			socket.sendData(&data);
			BOOST_REQUIRE(data.usefulDataSize() == 0);
			serverDone = true;
		},
		[&]() {
			TcpSocket socket(g_threadPool);
			socket.connect(endpoint);
			Buffer data({ 0x01, 0x02, 0x03, 0x04 });
			socket.sendData(&data);
			BOOST_REQUIRE(data.usefulDataSize() == 0);
			socket.receiveData(&data);
			BOOST_REQUIRE(data == test_data);
			clientDone = true;
		}
	}, [](const std::exception& exception) {
		BOOST_REQUIRE(false);
	});

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_CASE(TestNetworkIterator) {
	bool serverDone = false, clientDone = false;

	g_threadPool.parallel({
		[&]() {
			TcpServer server(g_threadPool, endpoint);
			TcpSocket socket = server.accept();
			{
				Buffer buffer({ 0x01, 0x02, 0x03, 0x04 });
				socket.sendData(&buffer);
			}
			{
				Buffer buffer({ 0x05, 0x06, 0x07, 0x08 });
				socket.sendData(&buffer);
			}
			serverDone = true;
		},
		[&]() {
			TcpSocket socket(g_threadPool);
			socket.connect(endpoint);
			Buffer buffer(1000);
			BOOST_REQUIRE(std::equal(
				NetworkIterator(socket, buffer),
				NetworkIterator(socket, buffer) + 4,
				test_data.begin()
			));
			BOOST_REQUIRE(std::equal(
				test_data2.begin(),
				test_data2.end(),
				NetworkIterator(socket, buffer.end())
			));
			BOOST_REQUIRE(buffer.usefulDataSize() == 8);
			clientDone = true;
		}
	}, [](const std::exception& exception) {
		BOOST_REQUIRE(false);
	});

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_SUITE_END()
