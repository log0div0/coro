
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
	error_code errorCode;

	auto coro = Coro::current();

	auto callback = [&](const error_code& errorCode_) {
		if (errorCode_) {
			errorCode = errorCode_;
		}
		if (coro) {
			coro->resume();
		}
	};

	_handle.async_connect(endpoint, coro->strand()->wrap(callback));

	try {
		coro->yield();
	}
	catch (...) {
		coro = nullptr;
		_handle.cancel();
		throw;
	}

	if (errorCode) {
		throw system_error(errorCode);
	}
}
