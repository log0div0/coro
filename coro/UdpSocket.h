
#pragma once

#include "coro/Buffer.h"
#include "coro/Mutex.h"

class UdpSocket {
public:
	UdpSocket();
	UdpSocket(const boost::asio::ip::udp::endpoint& endpoint);

	UdpSocket(UdpSocket&& other);
	UdpSocket& operator=(UdpSocket&& other);

	size_t send(const Buffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
	size_t receive(Buffer* buffer, boost::asio::ip::udp::endpoint* endpoint);

private:
	boost::asio::ip::udp::socket _handle;
	Mutex _receiveMutex, _sendMutex;
};