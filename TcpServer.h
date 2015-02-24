
#pragma once

#include "TcpSocket.h"
#include "CoroPool.h"

class TcpServer: public CoroPool {
public:
	TcpServer(const TcpEndpoint& endpoint);

	TcpSocket accept();

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
};
