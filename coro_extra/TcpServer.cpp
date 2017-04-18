
#include "coro_extra/TcpServer.h"
#include "coro/CoroPool.h"

using namespace asio::ip;
using namespace coro;

namespace coro {

void TcpServer::run(std::function<void(tcp::socket)> callback) {
	CoroPool coroPool;
	while (true) {
		auto socket = accept();
		coroPool.exec([&] {
			callback(std::move(socket));
		});
	}
}

}