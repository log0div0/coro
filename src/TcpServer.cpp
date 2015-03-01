
#include "TcpServer.h"
#include "ThreadPool.h"
#include "Coro.h"

using boost::system::error_code;
using boost::system::system_error;

TcpServer::TcpServer(const TcpEndpoint& endpoint)
	: _acceptor(ThreadPool::current()->ioService(), endpoint),
	  _socket(ThreadPool::current()->ioService()) {

}

TcpSocket TcpServer::accept() {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		_acceptor.async_accept(_socket, [&](const error_code& ec) {
			if (ec) {
				errorCode = ec;
			}
			coro.resume();
		});
	});

	if (errorCode) {
		throw system_error(errorCode);
	}

	return TcpSocket(std::move(_socket));
}
