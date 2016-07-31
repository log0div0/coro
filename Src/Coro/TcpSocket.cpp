
#include "coro/TcpSocket.h"
#include "coro/IoService.h"

using namespace asio::ip;

namespace coro {

TcpSocket::TcpSocket(): TcpSocket(tcp::socket(*IoService::current())) {

}

TcpSocket::TcpSocket(tcp::socket socket): Stream(std::move(socket)) {

}


TcpSocket::TcpSocket(TcpSocket&& other): Stream(std::move(other)) {

}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) {
	Stream::operator=(std::move(other));
	return *this;
}

void TcpSocket::connect(const tcp::endpoint& endpoint) {
	AsioTask1 task;
	_handle.async_connect(endpoint, task.callback());
	task.wait(_handle);
}

}