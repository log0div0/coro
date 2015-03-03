
#include "TcpSocket.h"
#include "ThreadPool.h"

using boost::system::error_code;
using boost::system::system_error;

TcpSocket::TcpSocket()
	: TcpSocket(std::move(boost::asio::ip::tcp::socket(ThreadPool::current()->ioService()))) {

}

void TcpSocket::connect(const TcpEndpoint& endpoint) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		_handle.async_connect(endpoint, [&](const error_code& errorCode_) {
			if (errorCode_) {
				errorCode = errorCode_;
			}
			coro.resume();
		});
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}

TcpSocket::TcpSocket(boost::asio::ip::tcp::socket socket)
	: IoHandle(std::move(socket)) {

}
