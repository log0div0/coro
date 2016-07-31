
#pragma once

#include "coro/Stream.h"

namespace coro {

/// Wrapper вокруг asio::ip::tcp::socket
class TcpSocket: public Stream<asio::ip::tcp::socket> {
public:
	TcpSocket();
	TcpSocket(asio::ip::tcp::socket socket);

	TcpSocket(TcpSocket&& other);
	TcpSocket& operator=(TcpSocket&& other);

	void connect(const asio::ip::tcp::endpoint& endpoint);
};

}