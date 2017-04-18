
#pragma once

#include "coro_extra/UdpSocket.h"
#include "coro/Queue.h"
#include <map>

namespace coro {

class UdpServer;

class UdpServerConnection {
public:
	UdpServerConnection(UdpServer* server, std::unique_ptr<Buffer> firstPacket,
		const asio::ip::udp::endpoint& endpoint);
	UdpServerConnection(const UdpServerConnection& other) = delete;
	UdpServerConnection(UdpServerConnection&& other);
	~UdpServerConnection();

	UdpServerConnection& operator=(const UdpServerConnection& other) = delete;
	UdpServerConnection& operator=(UdpServerConnection&& other);

	void send(std::unique_ptr<Buffer> buffer);
	std::unique_ptr<Buffer> receive();

	asio::ip::address_v4 remoteAddress() const {
		return _endpoint.address().to_v4();
	}

	uint16_t remotePort() const {
		return _endpoint.port();
	}

private:
	UdpServer* _server;
	asio::ip::udp::endpoint _endpoint;
	Queue<std::unique_ptr<Buffer>>* _inputQueue;
};

class UdpServer {
	friend class UdpServerConnection;
public:
	UdpServer(const asio::ip::udp::endpoint& endpoint);

	UdpServerConnection accept();
	void run(std::function<void(UdpServerConnection)> callback);

private:
	std::map<asio::ip::udp::endpoint, Queue<std::unique_ptr<Buffer>>> _inputQueues; //< TODO: std::unordered_map
	UdpSocket _socket;
};

}