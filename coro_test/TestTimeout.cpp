
#include "coro/Timeout.h"
#include "coro/Queue.h"
#include "coro/Mutex.h"
#include "coro/Acceptor.h"
#include "coro/StreamSocket.h"
#include "coro/DatagramSocket.h"
#include <catch.hpp>
#include <thread>

using namespace asio::ip;
using namespace coro;

TEST_CASE("A basic Timeout test", "[Timeout]") {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	REQUIRE_THROWS_AS(coro->yield({TokenThrow}), TimeoutError);
}


TEST_CASE("A basic TimeoutError test", "[Timeout]") {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	try {
		coro->yield({TokenThrow});
	}
	catch (const TimeoutError& error) {
		REQUIRE(error.timeout() == &timeout);
	}
}


TEST_CASE("The timeout fired when the coro was busy", "[Timeout]") {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	REQUIRE_THROWS_AS(coro->yield({TokenThrow}), TimeoutError);
}


TEST_CASE("Cancel timeout", "[Timeout]") {
	auto coro = Coro::current();

	{
		Timeout timeout(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	Strand::current()->post([&] {
		coro->resume("test");
	});
	REQUIRE_NOTHROW(coro->yield({"test"}));
}


TEST_CASE("Multiple timeouts", "[Timeout]") {
	auto coro = Coro::current();

	Timeout timeout(std::chrono::milliseconds(100));
	Timeout timeout2(std::chrono::milliseconds(100));
	REQUIRE_THROWS_AS(coro->yield({TokenThrow}), TimeoutError);
	REQUIRE_THROWS_AS(coro->yield({TokenThrow}), TimeoutError);
}


TEST_CASE("Timeout + queue", "[Timeout]") {
	Timeout timeout(std::chrono::milliseconds(100));
	Queue<uint64_t> queue;
	REQUIRE_THROWS_AS(queue.pop(), TimeoutError);
	queue.push(0);
}


TEST_CASE("Timeout + acceptor", "[Timeout]") {
	Timeout timeout(std::chrono::milliseconds(100));
	auto endpoint = tcp::endpoint(address::from_string("127.0.0.1"), 44442);
	Acceptor<tcp> acceptor(endpoint);
	REQUIRE_THROWS_AS(acceptor.accept(), TimeoutError);
}


TEST_CASE("Timeout + TCP socket", "[Timeout]") {
	Timeout timeout(std::chrono::milliseconds(100));
	auto endpoint = tcp::endpoint(address::from_string("127.0.0.1"), 44442);
	Acceptor<tcp> acceptor(endpoint);
	StreamSocket<tcp> socket;
	socket.connect(endpoint);
	std::vector<uint8_t> buffer(10);
	REQUIRE_THROWS_AS(socket.read(asio::buffer(buffer)), TimeoutError);
}

TEST_CASE("Timeout + UDP socket", "[Timeout]") {
	Timeout timeout(std::chrono::milliseconds(100));
	DatagramSocket<udp> socket(udp::endpoint(address::from_string("127.0.0.1"), 44442));
	std::vector<uint8_t> buffer(10);
	udp::endpoint endpoint;
	REQUIRE_THROWS_AS(socket.receive(asio::buffer(buffer), endpoint), TimeoutError);
}
