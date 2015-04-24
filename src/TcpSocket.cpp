
#include "TcpSocket.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpSocket::TcpSocket()
	: TcpSocket(tcp::socket(ThreadPool::current()->ioService())) {

}

TcpSocket::TcpSocket(tcp::socket socket): IoHandle(std::move(socket)) {

}


TcpSocket::TcpSocket(TcpSocket&& other): IoHandle(std::move(other)) {

}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) {
	IoHandle::operator=(std::move(other));
	return *this;
}

void TcpSocket::connect(const tcp::endpoint& endpoint) {
	auto task = std::make_shared<AsioTask1>();
	_handle.async_connect(endpoint, task->callback());
	task->wait(_handle);
}
