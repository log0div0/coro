
#pragma once

#include <boost/asio.hpp>

class TcpServer {
public:
	TcpServer(const boost::asio::ip::tcp::endpoint& endpoint);

	boost::asio::ip::tcp::socket accept();
	void run(std::function<void(boost::asio::ip::tcp::socket)> callback);

private:
	boost::asio::ip::tcp::acceptor _handle;
};