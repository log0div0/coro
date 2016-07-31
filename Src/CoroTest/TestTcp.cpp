
#include "Coro/Acceptor.h"
#include "Coro/TcpSocket.h"
#include "Coro/CoroPool.h"
#include <catch.hpp>

using namespace asio::ip;
using namespace coro;

static auto endpoint = tcp::endpoint(address::from_string("127.0.0.1"), 44442);
static std::vector<uint8_t> TestData { 0x01, 0x02, 0x03, 0x04 };

TEST_CASE("TCP server + TCP client") {
	bool serverDone = false, clientDone = false;

	CoroPool pool;
	pool.exec([&] {
		Acceptor acceptor(endpoint);
		TcpSocket socket = acceptor.accept();
		std::vector<uint8_t> data(4);
		REQUIRE(socket.read(asio::buffer(data)) == 4);
		REQUIRE(data == TestData);
		REQUIRE(socket.write(asio::buffer(data)) == 4);
		serverDone = true;
	});
	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);
		REQUIRE(socket.write(asio::buffer(TestData)) == 4);
		std::vector<uint8_t> data(4);
		REQUIRE(socket.read(asio::buffer(data)) == 4);
		REQUIRE(data == TestData);
		clientDone = true;
	});
	REQUIRE_NOTHROW(pool.waitAll(false));

	REQUIRE(serverDone);
	REQUIRE(clientDone);
}


TEST_CASE("Cancel Acceptor::accept") {
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
	REQUIRE_NOTHROW(pool.waitAll(false));

	REQUIRE(success);
}