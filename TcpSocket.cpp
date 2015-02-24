
#include "TcpSocket.h"
#include "ThreadPool.h"
#include "Coro.h"

using boost::system::error_code;
using boost::system::system_error;

TcpSocket::TcpSocket()
	: TcpSocket(std::move(boost::asio::ip::tcp::socket(ThreadPool::current()->ioService()))) {

}

TcpSocket::TcpSocket(TcpSocket&& other): _socket(std::move(other._socket)) {

}

TcpSocket& TcpSocket::operator=(TcpSocket&& other) {
	_socket = std::move(other._socket);
	return *this;
}

void TcpSocket::connect(const TcpEndpoint& endpoint) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		_socket.async_connect(endpoint, [&](const error_code& ec) {
			if (ec) {
				errorCode = ec;
			}
			coro.resume();
		});
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}

void TcpSocket::sendData(Buffer* buffer) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		boost::asio::async_write(_socket, buffer->usefulData(),
			[&](const error_code& ec, size_t bytes_tranfered) {
				if (ec) {
					errorCode = ec;
				} else {
					buffer->popFront(bytes_tranfered);
				}
				coro.resume();
			}
		);
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}

void TcpSocket::receiveData(Buffer* buffer) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		boost::asio::async_read(_socket, buffer->freeSpace(),
			[&](const error_code& ec, size_t bytes_tranfered) {
				if (ec) {
					errorCode = ec;
				} else {
					buffer->pushBack(bytes_tranfered);
				}
				coro.resume();
			}
		);
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}

void TcpSocket::receiveSomeData(Buffer* buffer) {
	Coro& coro = *Coro::current();
	error_code errorCode;

	coro.yield([&]() {
		_socket.async_read_some(buffer->freeSpace(),
			[&](const error_code& ec, size_t bytes_tranfered) {
				if (ec) {
					errorCode = ec;
				} else {
					buffer->pushBack(bytes_tranfered);
				}
				coro.resume();
			}
		);
	});

	if (errorCode) {
		throw system_error(errorCode);
	}
}

TcpSocket::TcpSocket(boost::asio::ip::tcp::socket socket)
	: _socket(std::move(socket)) {

}
