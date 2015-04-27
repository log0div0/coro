
#include <boost/test/unit_test.hpp>
#include "Timeout.h"
#include "Queue.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include <thread>


using namespace std::chrono_literals;
using namespace boost::asio::ip;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);


BOOST_AUTO_TEST_SUITE(SuiteTimeout)


BOOST_AUTO_TEST_CASE(TestCoroIsInactive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestTimeoutId) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	try {
		coro->yield();
	}
	catch (const TimeoutError& error) {
		BOOST_REQUIRE(error.timeoutId() == timeout.id());
	}
}


BOOST_AUTO_TEST_CASE(TestCoroIsActive) {
	auto coro = Coro::current();

	Timeout timeout(100ms);

	std::this_thread::sleep_for(200ms);

	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestCancel) {
	auto coro = Coro::current();

	{
		Timeout timeout(100ms);
	}

	std::this_thread::sleep_for(200ms);

	IoService::current()->post([&] {
		coro->resume();
	});
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


BOOST_AUTO_TEST_CASE(TestCancel2) {
	auto coro = Coro::current();

	{
		Timeout timeout(100ms);

		std::this_thread::sleep_for(200ms);
	}

	// Таймфут сработал, но он нам уже не нужен

	IoService::current()->post([&] {
		coro->resume();
	});
	BOOST_REQUIRE_NO_THROW(coro->yield());
}


BOOST_AUTO_TEST_CASE(TestTwoTimeouts) {
	auto coro = Coro::current();

	Timeout timeout(100ms);
	Timeout timeout2(100ms);
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
	BOOST_REQUIRE_THROW(coro->yield(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestQueue) {
	Timeout timeout(100ms);
	Queue<uint64_t> queue;
	BOOST_REQUIRE_THROW(queue.pop(), TimeoutError);
	queue.push(0);
}


BOOST_AUTO_TEST_CASE(TestTcpServerAccept) {
	Timeout timeout(100ms);
	TcpServer server(endpoint);
	BOOST_REQUIRE_THROW(server.accept(), TimeoutError);
}


BOOST_AUTO_TEST_CASE(TestTcpSocketRead) {
	Timeout timeout(100ms);
	TcpServer server(endpoint);
	TcpSocket socket;
	socket.connect(endpoint);
	Buffer buffer;
	BOOST_REQUIRE_THROW(socket.readSome(&buffer), TimeoutError);
}


BOOST_AUTO_TEST_SUITE_END()
