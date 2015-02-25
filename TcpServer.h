
#pragma once

#include "TcpSocket.h"
#include "CoroPool.h"

class TcpServer {
public:
	TcpServer(const TcpEndpoint& endpoint);

	TcpSocket accept();
	void run(std::function<void(TcpSocket socket)> routine);

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
	CoroPool _coroPool;
};
