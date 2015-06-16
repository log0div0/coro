
#include <boost/test/unit_test.hpp>
#include "coro/TcpServer.h"
#include "coro/TcpSocket.h"
#include "coro/CoroPool.h"

using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteTcp)


BOOST_AUTO_TEST_CASE(TestTcpSocketAndServer) {
	bool serverDone = false, clientDone = false;

	CoroPool pool;
	pool.exec([&] {
		TcpServer server(endpoint);
		TcpSocket socket = server.accept();
		Buffer data;
		data.pushBack(socket.readSome(&data));
		BOOST_REQUIRE(data == test_data);
		socket.write(data);
		serverDone = true;
	});
	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);
		socket.write(test_data);
		Buffer data;
		data.pushBack(socket.readSome(&data));
		BOOST_REQUIRE(data == test_data);
		clientDone = true;
	});
	pool.join();

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_CASE(TestCancelAccept) {
	bool success = false;

	CoroPool pool;
	pool.exec([&] {
		TcpServer server(endpoint);
		try {
			server.accept();
		}
		catch (const CancelError&) {
			success = true;
		}
	})->cancel();
	pool.join();

	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_SUITE_END()
