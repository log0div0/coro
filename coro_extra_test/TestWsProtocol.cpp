#include "coro/Acceptor.h"
#include "coro/CoroPool.h"
#include "coro_extra/WsProtocol.h"
#include "coro_extra/TcpSocket.h"
#include <catch.hpp>

using namespace asio::ip;
using namespace coro;

typedef StreamIterator<TcpSocket> SocketStreamIterator;

static auto endpoint = tcp::endpoint(address_v4::from_string("127.0.0.1"), 44442);
static std::vector<uint8_t> message_sample { 0x30, 0x31, 0x32 };

TEST_CASE("wsprotocol") {
	bool serverDone = false;
	bool clientDone = false;
	CoroPool pool;

	pool.exec([&] {
		Acceptor<tcp> acceptor(endpoint);
		TcpSocket socket = acceptor.accept();

		WsProtocol protocol;
		Buffer inputbuf;
		Buffer outbuf;

		inputbuf.popFront(
			protocol.doHandshake(
				socket.iterator(inputbuf),
				socket.iterator(),
				outbuf
			)
		);
		socket.write(outbuf);
		auto headers = protocol.handshakeHeaders();
		auto c1 = headers.find("CustomHeader1");
		REQUIRE(c1 != headers.end());
		REQUIRE(c1->second == "V1");
		auto c2 = headers.find("CustomHeader2");
		REQUIRE(c2 != headers.end());
		REQUIRE(c2->second == "V2");

		auto msg = protocol.readMessage(socket.iterator(inputbuf), socket.iterator());
		REQUIRE(msg.opCode() == WsMessage::OpCode::Binary);
		REQUIRE(msg.payloadLength() == message_sample.size());
		REQUIRE(std::equal(msg.payloadBegin(), msg.payloadEnd(), message_sample.begin()));

		outbuf.clear();
		outbuf.pushBack(message_sample.begin(), message_sample.end());
		protocol.writeMessage(WsMessage::OpCode::Binary, outbuf);
		for (size_t i=0; i<30; ++i) {
			socket.write(outbuf);
		}

		serverDone = true;
	});

	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);

		WsProtocol protocol;
		Buffer inputbuf;
		Buffer outbuf;

		HttpHeaders headers;
		headers.insert({"CustomHeader1", "V1"});
		headers.insert({"CustomHeader2", "V2"});
		protocol.writeHandshakeRequest("/wsk", headers, outbuf);
		socket.write(outbuf);

		inputbuf.popFront(
			protocol.readHandshakeResponse(
				socket.iterator(inputbuf),
				socket.iterator()
			)
		);

		outbuf.clear();
		outbuf.pushBack(message_sample.begin(), message_sample.end());
		protocol.writeMessage(WsMessage::OpCode::Binary, outbuf);
		socket.write(outbuf);

		for (size_t i=0; i<30; ++i) {
			auto msg = protocol.readMessage(socket.iterator(inputbuf), socket.iterator());
			REQUIRE(msg.opCode() == WsMessage::OpCode::Binary);
			REQUIRE(msg.payloadLength() == message_sample.size());
			REQUIRE(std::equal(msg.payloadBegin(), msg.payloadEnd(), message_sample.begin()));
			inputbuf.popFront(msg.end());
		}

		clientDone = true;
	});

	REQUIRE_NOTHROW(pool.waitAll(false));
	REQUIRE(serverDone);
	REQUIRE(clientDone);
}

