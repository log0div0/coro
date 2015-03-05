
#pragma once

#include "CoroPool.h"
#include <boost/asio.hpp>

class TcpServer {
public:
	TcpServer(const boost::asio::ip::tcp::endpoint& endpoint);

	boost::asio::ip::tcp::socket accept();

	template <typename T>
	void run() {
		while (true) {
			auto session = std::make_shared<T>(accept());
			_coroPool.exec([session = std::move(session)]() {
				(*session)();
			});
		}
	}

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	CoroPool _coroPool;
};
