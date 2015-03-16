
#pragma once

#include "CoroPool.h"
#include <boost/asio.hpp>
#include <mutex>

class TcpServer {
public:
	TcpServer(const boost::asio::ip::tcp::endpoint& endpoint);

	boost::asio::ip::tcp::socket accept();
	void run(std::function<void(boost::asio::ip::tcp::socket)> callback);
	void shutdown();

private:
	CoroPool _coroPool;
	boost::asio::ip::tcp::acceptor _acceptor;
	std::mutex _mutex;
	bool _shutdown = false;
};
