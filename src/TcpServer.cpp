
#include "TcpServer.h"
#include "AsioTask.h"
#include "IoService.h"
#include "CoroPool.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpServer::TcpServer(const tcp::endpoint& endpoint): _handle(*IoService::current(), endpoint)
{
	boost::asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
}

tcp::socket TcpServer::accept() {
	tcp::socket socket(*IoService::current());

	AsioTask1 task;
	_handle.async_accept(socket, task.callback());
	task.wait(_handle);

	return socket;
}

void TcpServer::run(std::function<void(tcp::socket)> callback) {
	CoroPool pool;
	while (true) {
		auto socket = std::make_shared<tcp::socket>(accept());
		pool.exec([=]() {
			callback(std::move(*socket));
		});
	}
}
