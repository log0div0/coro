
#include <boost/test/unit_test.hpp>
#include "coro/Acceptor.h"
#include "coro/TcpSocket.h"
#include "coro/CoroPool.h"

using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static std::vector<uint8_t> TestData { 0x01, 0x02, 0x03, 0x04 };


BOOST_AUTO_TEST_SUITE(SuiteTcp)


BOOST_AUTO_TEST_CASE(TestTcpSocketAndServer) {
	bool serverDone = false, clientDone = false;

	CoroPool pool;
	pool.exec([&] {
		Acceptor acceptor(endpoint);
		TcpSocket socket = acceptor.accept();
		std::vector<uint8_t> data(4);
		BOOST_REQUIRE(socket.read(boost::asio::buffer(data)) == 4);
		BOOST_REQUIRE(data == TestData);
		BOOST_REQUIRE(socket.write(boost::asio::buffer(data)) == 4);
		serverDone = true;
	});
	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);
		BOOST_REQUIRE(socket.write(boost::asio::buffer(TestData)) == 4);
		std::vector<uint8_t> data(4);
		BOOST_REQUIRE(socket.read(boost::asio::buffer(data)) == 4);
		BOOST_REQUIRE(data == TestData);
		clientDone = true;
	});
	pool.waitAll();

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_CASE(TestCancelAccept) {
	bool success = false;

	CoroPool pool;
	pool.exec([&] {
		Acceptor acceptor(endpoint);
		try {
			acceptor.accept();
		}
		catch (const CancelError&) {
			success = true;
		}
	})->cancel();
	pool.waitAll();

	BOOST_REQUIRE(success);
}


BOOST_AUTO_TEST_SUITE_END()
