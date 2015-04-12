
#include <boost/test/unit_test.hpp>
#include "TcpServer.h"
#include "TcpSocket.h"
#include "CoroPool.h"
#include <iostream>


using namespace std;
using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteTcp)


BOOST_AUTO_TEST_CASE(TestAcceptConnect) {
	const auto iterations = 1000;
	Exec([&] {
		TcpServer server(endpoint);
		for (auto i = 0; i < iterations; i++) {
			server.accept();
		}
	});
	Exec([&] {
		for (auto i = 0; i < iterations; i++) {
			TcpSocket socket;
			socket.connect(endpoint);
		}
	});
	Join();
}


BOOST_AUTO_TEST_CASE(TestWriteRead) {
	const auto iterations = 1000;
	Exec([&] {
		TcpServer server(endpoint);
		TcpSocket socket = server.accept();
		for (auto i = 0; i < iterations; i++) {
			try {
				Buffer buffer;
				while (true) {
					socket.read(&buffer);
				}
			}
			catch (...) {}
		}
	});
	Exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);
		for (auto i = 0; i < iterations; i++) {
			socket.write(test_data);
		}
	});
	Join();
}


BOOST_AUTO_TEST_CASE(TestComplex) {
	const auto iterations = 1000;
	bool success = true;
	for (auto i = 0; i < iterations; i++) {
		cout << "SuiteTcp/TestComplex " << i << " of " << iterations << endl;
		TcpServer server(endpoint);
		CoroPool serverPool;
		serverPool.exec([&]() {
			try {
				server.run([&](TcpSocket socket) {
					try {
						Buffer buffer;
						while (true) {
							socket.read(&buffer);
						}
					}
					catch (const boost::system::system_error& error) {
						if (error.code() != boost::asio::error::eof) {
							success = false;
						}
					}
				});
			}
			catch (const boost::system::system_error& error) {
				if (error.code() != boost::system::errc::operation_canceled) {
					success = false;
				}
			}
			catch (...) {
				success = false;
			}
		});
		{
			CoroPool clientPool;
			for (auto i = 0; i < 1000; ++i) {
				clientPool.exec([&]() {
					try {
						TcpSocket socket;
						socket.connect(endpoint);
						socket.write(test_data);
					}
					catch (...) {
						success = false;
					}
				});
			}
		}
		server.shutdown();
	}
	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_SUITE_END()
