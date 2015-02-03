
#include <boost/test/unit_test.hpp>
#include "TcpServer.h"


extern ThreadPool g_threadPool;


BOOST_AUTO_TEST_SUITE(SuiteTcp)


BOOST_AUTO_TEST_CASE(TestTransferData) {

	DynamicBuffer test_data({ 0x01, 0x02, 0x03, 0x04 });
	auto endpoint = TcpEndpoint(IPv4Address::from_string("127.0.0.1"), 44442);

	g_threadPool.parallel({
		[&]() {
			TcpServer server(g_threadPool, endpoint);
			TcpSocket socket = server.accept();
			DynamicBuffer data(4);
			socket.receiveData(&data);
			BOOST_REQUIRE(data.isEqual(test_data));
			socket.sendData(&test_data);
		},
		[&]() {
			TcpSocket socket(g_threadPool);
			socket.connect(endpoint);
			socket.sendData(&test_data);
			DynamicBuffer data(4);
			socket.receiveData(&data);
			BOOST_REQUIRE(data.isEqual(test_data));
		}
	}, [](const std::exception& exception) {
		BOOST_REQUIRE(false);
	});
}


BOOST_AUTO_TEST_SUITE_END()
