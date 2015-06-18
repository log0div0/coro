
#include "coro/TcpServer.h"
#include "coro/AsioTask.h"
#include "coro/IoService.h"
#include "coro/CoroPool.h"

using boost::system::error_code;
using boost::system::system_error;
using namespace boost::asio::ip;

TcpServer::TcpServer(const tcp::endpoint& endpoint): _handle(*IoService::current())
{
	_handle.open(endpoint.protocol());
	boost::asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
	_handle.bind(endpoint);
	_handle.listen();
}

tcp::socket TcpServer::accept() {
	tcp::socket socket(*IoService::current());

	AsioTask1 task;
	_handle.async_accept(socket, task.callback());
	task.wait(_handle);

	return socket;
}

void TcpServer::run(std::function<void(tcp::socket)> callback) {
	CoroPool coroPool;
	while (true) {
		coroPool.exec([&]() {
			callback(accept());
		});
	}
}
