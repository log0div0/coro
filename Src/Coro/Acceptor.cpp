
#include "Coro/Acceptor.h"
#include "Coro/AsioTask.h"
#include "Coro/IoService.h"
#include "Coro/CoroPool.h"

using namespace asio::ip;

namespace coro {

Acceptor::Acceptor(const tcp::endpoint& endpoint): _handle(*IoService::current())
{
	_handle.open(endpoint.protocol());
	asio::socket_base::reuse_address option(true);
	_handle.set_option(option);
	_handle.bind(endpoint);
	_handle.listen();
}

tcp::socket Acceptor::accept() {
	tcp::socket socket(*IoService::current());

	AsioTask1 task;
	_handle.async_accept(socket, task.callback());
	task.wait(_handle);

	return socket;
}

void Acceptor::run(std::function<void(tcp::socket)> callback) {
	CoroPool coroPool;
	while (true) {
		auto socket = accept();
		coroPool.exec([&] {
			callback(std::move(socket));
		});
	}
}

}