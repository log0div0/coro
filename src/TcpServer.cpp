
#include "TcpServer.h"
#include "ThreadPool.h"
#include "Coro.h"

using boost::system::error_code;
using boost::system::system_error;

TcpServer::TcpServer(const boost::asio::ip::tcp::endpoint& endpoint)
	: _acceptor(ThreadPool::current()->ioService(), endpoint)
{

}

boost::asio::ip::tcp::socket TcpServer::accept() {
	boost::asio::ip::tcp::socket socket(ThreadPool::current()->ioService());

	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		_acceptor.async_accept(socket, [&](const error_code& ec) {
			if (ec) {
				errorCode = ec;
			}
			coro.resume();
		});
	});

	if (errorCode) {
		throw system_error(errorCode);
	}

	return socket;
}
