
#include "TcpServer.h"

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

	tcp::socket socket(ThreadPool::current()->ioService());

	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (!_shutdown) {
			_acceptor.async_accept(socket, coro->strand()->wrap(callback));
		} else {
			throw system_error(make_error_code(boost::system::errc::operation_canceled));
		}
	}

	try {
		task->coro->yield();
	}
	catch (...) {
		task->coro = nullptr;
		_acceptor.cancel();
		throw;
	}

	if (task->errorCode) {
		throw system_error(task->errorCode);
	}

	return socket;
}

void TcpServer::run(std::function<void(tcp::socket)> callback) {
	while (true) {
		auto socket = std::make_shared<tcp::socket>(accept());
		_coroPool.exec([=]() {
			callback(std::move(*socket));
		});
	}
}

void TcpServer::shutdown() {
	std::lock_guard<std::mutex> lock(_mutex);
	_shutdown = true;
	_acceptor.cancel();
}