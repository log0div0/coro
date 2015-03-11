
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
		TcpServer server(endpoint);
		Exec([&]() {
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
				BOOST_REQUIRE(error.code() == boost::system::errc::operation_canceled);
			}
			catch (...) {
				BOOST_REQUIRE(false);
			}
		});
		CoroPool pool;
		for (auto i = 0; i < 1024; ++i) {
			pool.exec([&]() {
				TcpSocket socket;
				socket.connect(endpoint);
				for (auto i = 0; i < 8; i++) {
					socket.write(test_data);
				}
			});
		}
		pool.join();
		server.cancel();
		Join();
	}
}


BOOST_AUTO_TEST_SUITE_END()
