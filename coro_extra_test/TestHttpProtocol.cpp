#include "coro/Acceptor.h"
#include "coro/CoroPool.h"
#include "coro_extra/HttpProtocol.h"
#include "coro_extra/TcpSocket.h"
#include <catch.hpp>

using namespace asio::ip;
using namespace coro;

typedef StreamIterator<TcpSocket> SocketStreamIterator;

TEST_CASE("httpprotocol") {
	auto endpoint = tcp::endpoint(address_v4::from_string("104.23.131.81"), 80);

	CoroPool pool;
	bool clientDone = false;

	pool.exec([&] {
		TcpSocket socket;
		socket.connect(endpoint);

		HttpProtocol protocol;
		Buffer inbuf;
		Buffer outbuf;
		Buffer body;
		HttpHeaders headers;
		headers.insert({"User-Agent", "test util"});

		protocol.writeGET("http://www.animenewsnetwork.com/encyclopedia/anime.php?id=18671", headers, outbuf);
		socket.write(outbuf);

		inbuf.reserve(128*1024);
		body.reserve(128*1024);
		protocol.readResponse(socket.iterator(inbuf), socket.iterator(), body);
		REQUIRE(protocol.response().code == 200);

		clientDone = true;
	});

	REQUIRE_NOTHROW(pool.waitAll(false));
	REQUIRE(clientDone);
}

