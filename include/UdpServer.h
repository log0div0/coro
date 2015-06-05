
#pragma once

#include "UdpSocket.h"
#include "Queue.h"
#include "Mutex.h"

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
	void send(BufferUniquePtr buffer, const boost::asio::ip::udp::endpoint& endpoint);

	std::map<boost::asio::ip::udp::endpoint, Queue<BufferUniquePtr>> _inputQueues;
	UdpSocket _socket;
	Mutex _mutex;
};