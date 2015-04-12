
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
	auto coro = Coro::current();

	struct Task {
		Coro* coro;
		error_code errorCode;
	};

	auto task = std::make_shared<Task>();
	task->coro = coro;

	auto callback = [task](const error_code& errorCode) {
		if (!task->coro) {
			return;
		}
		if (errorCode) {
			task->errorCode = errorCode;
		}
		task->coro->resume();
	};

	_handle.async_connect(endpoint, coro->strand()->wrap(callback));

	try {
		task->coro->yield();
	}
	catch (...) {
		task->coro = nullptr;
		_handle.cancel();
		throw;
	}

	if (task->errorCode) {
		throw system_error(task->errorCode);
	}
}
