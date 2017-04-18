
#include "coro_extra/UdpSocket.h"
#include "coro/AsioTask.h"
#include "coro/IoService.h"

using namespace asio::ip;

namespace coro {

size_t UdpSocket::send(const Buffer& buffer, const udp::endpoint& endpoint) {
	std::lock_guard<Mutex> lock(_sendMutex);
	return DatagramSocket<udp>::send(buffer.usefulData(), endpoint);
}

size_t UdpSocket::receive(Buffer* buffer, udp::endpoint* endpoint) {
	std::lock_guard<Mutex> lock(_receiveMutex);
	return DatagramSocket<udp>::receive(buffer->freeSpace(), *endpoint);
}

}