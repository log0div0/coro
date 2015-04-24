
#pragma once

#include "IoHandle.h"
#include "CoroPool.h"

class TcpServer {
public:
	TcpServer(const boost::asio::ip::tcp::endpoint& endpoint);

	boost::asio::ip::tcp::socket accept();
	void run(std::function<void(boost::asio::ip::tcp::socket)> callback);

private:
	CoroPool _coroPool;
	boost::asio::ip::tcp::acceptor _handle;
};
