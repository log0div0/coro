
#include "UdpSocket.h"
#include "AsioTask.h"
#include "ioService.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

UdpSocket::UdpSocket(): _handle(*IoService::current(), udp::v4())
{

}

UdpSocket::UdpSocket(const udp::endpoint& endpoint): _handle(*IoService::current(), endpoint)
{

}

size_t UdpSocket::send(const Buffer& buffer, const udp::endpoint& endpoint) {
	AsioTask2 task;
	_handle.async_send_to(buffer.usefulData(), endpoint, task.callback());
	return task.wait(_handle);
}

size_t UdpSocket::receive(Buffer* buffer, udp::endpoint* endpoint) {
	AsioTask2 task;
	_handle.async_receive_from(buffer->freeSpace(), *endpoint, task.callback());
	return task.wait(_handle);
}
