
#include "UdpSocket.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

UdpSocket::UdpSocket()
	: _handle(ThreadPool::current()->ioService(), udp::v4())
{

}

UdpSocket::UdpSocket(const udp::endpoint& endpoint)
	: _handle(ThreadPool::current()->ioService(), endpoint)
{

}

size_t UdpSocket::send(const Buffer& buffer, const udp::endpoint& endpoint) {
	auto task = std::make_shared<AsioTask2>();
	_handle.async_send_to(buffer.usefulData(), endpoint, task->callback());
	return task->wait(_handle);
}

size_t UdpSocket::receive(Buffer* buffer, udp::endpoint* endpoint) {
	auto task = std::make_shared<AsioTask2>();
	_handle.async_receive_from(buffer->freeSpace(), *endpoint, task->callback());
	return task->wait(_handle);
}

udp::endpoint UdpSocket::endpoint() const {
	return _handle.local_endpoint();
}