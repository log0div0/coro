
#include <boost/test/unit_test.hpp>
#include "coro/Timeout.h"
#include "coro/Queue.h"
#include "coro/Acceptor.h"
#include "coro/TcpSocket.h"
#include "coro/UdpSocket.h"
#include <thread>


using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);


BOOST_AUTO_TEST_SUITE(SuiteTimeout)


BOOST_AUTO_TEST_CASE(TestCoroIsInactive) {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestTimeoutId) {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	try {
		coro->yield();
	}
	catch (const TimeoutError& error) {
		BOOST_REQUIRE(error.timeoutId() == timeout.id());
	}
}


BOOST_AUTO_TEST_CASE(TestCoroIsActive) {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCancel) {
	auto coro = Coro::current();

	{
		Timeout timeout(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	IoService::current()->post([&] {
		coro->resume();
	});
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


BOOST_AUTO_TEST_CASE(TestTwoTimeouts) {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));
	Timeout timeout2(std::chrono::milliseconds(100));
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestQueue) {
	Timeout timeout(std::chrono::milliseconds(100));
	Queue<uint64_t> queue;
	BOOST_REQUIRE_THROW(queue.pop(), TimeoutError);
	queue.push(0);
}


BOOST_AUTO_TEST_CASE(TestAcceptor) {
	Timeout timeout(std::chrono::milliseconds(100));
	Acceptor acceptor(endpoint);
	BOOST_REQUIRE_THROW(acceptor.accept(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestTcpSocketRead) {
	Timeout timeout(std::chrono::milliseconds(100));
	Acceptor acceptor(endpoint);
	TcpSocket socket;
	socket.connect(endpoint);
	std::vector<uint8_t> buffer(10);
	BOOST_REQUIRE_THROW(socket.read(boost::asio::buffer(buffer)), TimeoutError);
}

BOOST_AUTO_TEST_CASE(TestUdpSocketReceive) {
	Timeout timeout(std::chrono::milliseconds(100));
	UdpSocket socket(udp::endpoint(address_v4::from_string("127.0.0.1"), 44442));
	std::vector<uint8_t> buffer(10);
	udp::endpoint endpoint;
	BOOST_REQUIRE_THROW(socket.receive(boost::asio::buffer(buffer), endpoint), TimeoutError);
}


BOOST_AUTO_TEST_SUITE_END()
