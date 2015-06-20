
#include <boost/test/unit_test.hpp>
#include "coro/Acceptor.h"
#include "coro/TcpSocket.h"
#include "coro/StreamIterator.h"
#include "coro/CoroPool.h"


using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static std::string test_data("abcd");
static std::string test_data2("efgh");


BOOST_AUTO_TEST_SUITE(SuiteStreamIterator)


BOOST_AUTO_TEST_CASE(TestSTL) {
	bool serverDone = false, clientDone = false;

	CoroPool pool;
	pool.exec([&] {
		Acceptor acceptor(endpoint);
		TcpSocket socket = acceptor.accept();
		BOOST_REQUIRE(socket.write(boost::asio::buffer(test_data)) == 4);
		BOOST_REQUIRE(socket.write(boost::asio::buffer(test_data2)) == 4);
		serverDone = true;
	});
	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);
		std::string buffer;
		StreamIterator<TcpSocket, std::string> begin(socket, buffer), end;
		auto it = std::find(begin, end, 'd');
		buffer.erase(it + 1, buffer.end());
		BOOST_REQUIRE(buffer == test_data);
		clientDone = true;
	});
	pool.waitAll();

	BOOST_REQUIRE(serverDone && clientDone);
}


BOOST_AUTO_TEST_SUITE_END()
