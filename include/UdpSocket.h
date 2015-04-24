
#pragma once

#include "IoHandle.h"

class UdpSocket {
public:
	UdpSocket();
	UdpSocket(const boost::asio::ip::udp::endpoint& endpoint);

	size_t send(const Buffer& buffer, const boost::asio::ip::udp::endpoint& endpoint);
	size_t receive(Buffer* buffer, boost::asio::ip::udp::endpoint* endpoint);

	boost::asio::ip::udp::endpoint endpoint() const;

private:
	boost::asio::ip::udp::socket _handle;
};