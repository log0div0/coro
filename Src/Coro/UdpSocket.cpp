
#include "Coro/UdpSocket.h"
#include "Coro/AsioTask.h"
#include "Coro/IoService.h"

using namespace asio::ip;

namespace coro {

UdpSocket::UdpSocket(): _handle(*IoService::current(), udp::v4())
{

}

UdpSocket::UdpSocket(const udp::endpoint& endpoint): _handle(*IoService::current())
{
	_handle.open(endpoint.protocol());
	asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
	_handle.bind(endpoint);
}

UdpSocket::UdpSocket(UdpSocket&& other): _handle(std::move(other._handle)) {

}

UdpSocket& UdpSocket::operator=(UdpSocket&& other) {
	_handle = std::move(other._handle);
	return *this;
}

}