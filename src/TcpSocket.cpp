
#include "TcpSocket.h"
#include "ThreadPool.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpSocket::TcpSocket()
	: TcpSocket(tcp::socket(ThreadPool::current()->ioService())) {

}

TcpSocket::TcpSocket(tcp::socket socket): IoHandle(std::move(socket)) {

}

void TcpSocket::connect(const tcp::endpoint& endpoint) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	auto callback = [&](const error_code& errorCode_) {
		if (errorCode_) {
			errorCode = errorCode_;
		}
		coro.resume();
	};

	coro.yield([&]() {
		_handle.async_connect(endpoint, callback);
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}
