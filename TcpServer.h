
#pragma once

#include "TcpSocket.h"

class TcpServer {
public:
	TcpServer(ThreadPool& threadPool, const TcpEndpoint& endpoint);

	TcpSocket accept();

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::ip::tcp::socket _socket;
};