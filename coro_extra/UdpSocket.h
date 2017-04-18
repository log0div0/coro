
#pragma once

#include "coro/DatagramSocket.h"
#include "coro/Mutex.h"
#include "coro_extra/Buffer.h"

namespace coro {

class UdpSocket: DatagramSocket<asio::ip::udp> {
public:
	using DatagramSocket<asio::ip::udp>::DatagramSocket;

	size_t send(const Buffer& buffer, const asio::ip::udp::endpoint& endpoint);
	size_t receive(Buffer* buffer, asio::ip::udp::endpoint* endpoint);

private:
	Mutex _receiveMutex, _sendMutex;
};

}