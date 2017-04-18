
#pragma once

#include <coro/Acceptor.h>

namespace coro {

class TcpServer: Acceptor<asio::ip::tcp> {
public:
	using Acceptor<asio::ip::tcp>::Acceptor;

	void run(std::function<void(asio::ip::tcp::socket)> callback);
};

}