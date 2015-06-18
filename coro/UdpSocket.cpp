
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

size_t UdpSocket::send(const Buffer& buffer, const udp::endpoint& endpoint) {
	std::lock_guard<Mutex> lock(_sendMutex);
	AsioTask2<size_t> task;
	_handle.async_send_to(buffer.usefulData(), endpoint, task.callback());
	return task.wait(_handle);
}

size_t UdpSocket::receive(Buffer* buffer, udp::endpoint* endpoint) {
	std::lock_guard<Mutex> lock(_receiveMutex);
	AsioTask2<size_t> task;
	_handle.async_receive_from(buffer->freeSpace(), *endpoint, task.callback());
	return task.wait(_handle);
}
