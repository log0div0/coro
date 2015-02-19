
#pragma once

#include "ThreadPool.h"
#include "Buffer.h"

typedef boost::asio::ip::address_v4 IPv4Address;
typedef boost::asio::ip::tcp::endpoint TcpEndpoint;

class TcpSocket {
public:
	friend class TcpServer;

	TcpSocket(ThreadPool& threadPool);

	// VS2013 не умеет их генерить
	TcpSocket(TcpSocket&& other);

	void connect(const TcpEndpoint& endpoint);

	void sendData(Buffer* buffer);
	void receiveData(Buffer* buffer);

protected:
	TcpSocket(boost::asio::ip::tcp::socket socket);

private:
	boost::asio::ip::tcp::socket _socket;
};
