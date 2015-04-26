
#pragma once

#include "Stream.h"

class TcpSocket: public Stream<boost::asio::ip::tcp::socket> {
public:
	TcpSocket();
	TcpSocket(boost::asio::ip::tcp::socket socket);

	TcpSocket(TcpSocket&& other);
	TcpSocket& operator=(TcpSocket&& other);

	void connect(const boost::asio::ip::tcp::endpoint& endpoint);
};
