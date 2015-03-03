
#pragma once

#include "IoHandle.h"

typedef boost::asio::ip::address_v4 IPv4Address;
typedef boost::asio::ip::tcp::endpoint TcpEndpoint;

class TcpSocket: public IoHandle<boost::asio::ip::tcp::socket> {
public:
	friend class TcpServer;

	TcpSocket();

	void connect(const TcpEndpoint& endpoint);

protected:
	TcpSocket(boost::asio::ip::tcp::socket socket);
};
