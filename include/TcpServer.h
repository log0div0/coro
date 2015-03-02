
#pragma once

#include "TcpSocket.h"
#include "CoroPool.h"

class TcpServer {
public:
	TcpServer(const TcpEndpoint& endpoint);

	TcpSocket accept();

	template <typename T>
	void run() {
		while (true) {
			auto session = std::make_shared<T>(accept());
			_coroPool.exec([session = std::move(session)]() {
				(*session)();
			});
		}
	}

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
	CoroPool _coroPool;
};
