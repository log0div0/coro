
#include <boost/test/unit_test.hpp>
#include "coro/TcpServer.h"
#include "coro/TcpSocket.h"
#include "coro/CoroPool.h"


using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static Buffer test_data { 0x01, 0x02, 0x03, 0x04 };
static Buffer test_data2 { 0x05, 0x06, 0x07, 0x08 };


BOOST_AUTO_TEST_SUITE(SuiteTcpIterator)


BOOST_AUTO_TEST_CASE(TestSTL) {
	bool serverDone = false, clientDone = false;

	CoroPool pool;
	pool.exec([&] {
		TcpServer server(endpoint);
		TcpSocket socket = server.accept();
		socket.write(test_data);
		socket.write(test_data2);
		serverDone = true;
	});
	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);

		Buffer buffer;

		BOOST_REQUIRE(std::equal(
			test_data.begin(),
			test_data.end(),
			socket.iterator(buffer)
		));

		buffer.popFront(4);

		BOOST_REQUIRE(std::equal(
			socket.iterator(buffer),
			socket.iterator(buffer) + 4,
			test_data2.begin()
		));

		clientDone = true;
	});
	pool.waitAll();

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_CASE(TestCastToBufferIterator) {
	TcpSocket socket;
	Buffer buffer(8);
	auto it = socket.iterator(buffer);
	buffer.pushBack(test_data.begin(), test_data.end());
	BOOST_REQUIRE(*(it + 1) == *(buffer.begin() + 1));
	BOOST_REQUIRE(buffer.end() == (it + 4));
}


BOOST_AUTO_TEST_SUITE_END()
