
#pragma once

#include "coro/UdpSocket.h"
#include "coro/Queue.h"

class UdpServer;

class UdpServerConnection {
public:
	UdpServerConnection(UdpServer* server, const boost::asio::ip::udp::endpoint& endpoint);
	UdpServerConnection(const UdpServerConnection& other) = delete;
	UdpServerConnection(UdpServerConnection&& other);
	~UdpServerConnection();

	UdpServerConnection& operator=(const UdpServerConnection& other) = delete;
	UdpServerConnection& operator=(UdpServerConnection&& other);

	void send(BufferUniquePtr buffer);
	BufferUniquePtr receive();

	boost::asio::ip::address_v4 remoteAddress() const {
		return _endpoint.address().to_v4();
	}

	uint16_t remotePort() const {
		return _endpoint.port();
	}

private:
	UdpServer* _server;
	boost::asio::ip::udp::endpoint _endpoint;
	Queue<BufferUniquePtr>* _inputQueue;
};

class UdpServer {
	friend class UdpServerConnection;
public:
	UdpServer(const boost::asio::ip::udp::endpoint& endpoint);

	void run(std::function<void(UdpServerConnection)> callback);

private:
	std::map<boost::asio::ip::udp::endpoint, Queue<BufferUniquePtr>> _inputQueues;
	UdpSocket _socket;
};