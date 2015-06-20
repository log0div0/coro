
#include "coro/UdpSocket.h"
#include "coro/AsioTask.h"
#include "coro/IoService.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

UdpSocket::UdpSocket(): _handle(*IoService::current(), udp::v4())
{

}

UdpSocket::UdpSocket(const udp::endpoint& endpoint): _handle(*IoService::current())
{
	_handle.open(endpoint.protocol());
	boost::asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
	_handle.bind(endpoint);
}

UdpSocket::UdpSocket(UdpSocket&& other): _handle(std::move(other._handle)) {

}

UdpSocket& UdpSocket::operator=(UdpSocket&& other) {
	_handle = std::move(other._handle);
	return *this;
}
