
#include "TcpServer.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpServer::TcpServer(const tcp::endpoint& endpoint)
	: _handle(ThreadPool::current()->ioService(), endpoint)
{
	boost::asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
}


tcp::socket TcpServer::accept() {
	tcp::socket socket(ThreadPool::current()->ioService());

	auto task = std::make_shared<AsioTask1>();
	_handle.async_accept(socket, task->callback());
	task->wait(_handle);

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
