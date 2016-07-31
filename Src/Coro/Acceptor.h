
#pragma once

#include <functional>
#include <asio.hpp>

namespace coro {

/// Wrapper вокруг asio::ip::tcp::acceptor
class Acceptor {
public:
	Acceptor(const asio::ip::tcp::endpoint& endpoint);

	asio::ip::tcp::socket accept();
	/*!
		@brief В цикле принимает подключения и запускает их обработчики в отдельных Strand
		@param callback - функция-обработчик соединения
	*/
	void run(std::function<void(asio::ip::tcp::socket)> callback);

	asio::ip::tcp::acceptor& handle() {
		return _handle;
	}

private:
	asio::ip::tcp::acceptor _handle;
};

}