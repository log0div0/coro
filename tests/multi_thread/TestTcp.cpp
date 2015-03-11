
#include <boost/test/unit_test.hpp>
#include "TcpServer.h"
#include "TcpSocket.h"
#include "CoroPool.h"
#include <iostream>


using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteTcp)


BOOST_AUTO_TEST_CASE(TestTcpSocketAndServer) {
	for (auto i = 0; i < 10; i++) {
		CoroPool serverPool;
		TcpServer server(endpoint);
		serverPool.exec([&]() {
			try {
				server.run([&](TcpSocket socket) {
					Buffer buffer;
					while (true) {
						socket.read(&buffer);
						buffer.clear();
					}
				});
			}
			catch (const boost::system::system_error& error) {
				BOOST_REQUIRE(true);
			}
			catch (...) {
				BOOST_REQUIRE(false);
			}
		});
		CoroPool clientPool;
		for (auto i = 0; i < 1024; ++i) {
			clientPool.exec([&]() {
				TcpSocket socket;
				socket.connect(endpoint);
				for (auto i = 0; i < 10; i++) {
					socket.write(test_data);
				}
			});
		}
		printf("%d\n", i);
	}
}


BOOST_AUTO_TEST_SUITE_END()
