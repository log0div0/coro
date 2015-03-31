
#include "TcpServer.h"
#include "Task.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpServer::TcpServer(const tcp::endpoint& endpoint)
	: _acceptor(ThreadPool::current()->ioService(), endpoint)
{
	boost::asio::socket_base::reuse_address option(true);
	_acceptor.set_option(option);
}

tcp::socket TcpServer::accept() {
	Coro& coro = *Coro::current();
	error_code errorCode;

	auto callback = [&](const error_code& ec) {
		if (ec) {
			errorCode = ec;
		}
		coro.resume();
	};

	tcp::socket socket(ThreadPool::current()->ioService());

	coro.yield([&]() {
		std::lock_guard<std::mutex> lock(_mutex);
		if (!_shutdown) {
			_acceptor.async_accept(socket, callback);
		} else {
			callback(error_code(boost::system::errc::operation_canceled,
				boost::system::system_category()));
		}
	});

	if (errorCode) {
		throw system_error(errorCode);
	}

	return socket;
}

void TcpServer::run(std::function<void(tcp::socket)> callback) {
	while (true) {
		auto socket = std::make_shared<tcp::socket>(accept());
		_coroPool.exec([callback, socket]() {
			callback(std::move(*socket));
		});
	}
}

void TcpServer::shutdown() {
	std::lock_guard<std::mutex> lock(_mutex);
	_acceptor.cancel();
	_shutdown = true;
}