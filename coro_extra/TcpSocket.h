
#pragma once

#include "coro/StreamSocket.h"

#include "coro_extra/StreamIterator.h"
#include "coro_extra/Buffer.h"

namespace coro {

class TcpSocket: public StreamSocket<asio::ip::tcp> {
public:
	using StreamSocket<asio::ip::tcp>::StreamSocket;
	using StreamSocket<asio::ip::tcp>::write;
	using StreamSocket<asio::ip::tcp>::readSome;

	typedef StreamIterator<TcpSocket> Iterator;

	Iterator iterator(Buffer& buffer) {
		return Iterator(*this, buffer);
	}

	Iterator iterator() {
		return Iterator();
	}

	// Как бы тут получше разрешить конфликт с шаблоной write?
	size_t write(Buffer& buffer) { return write(buffer); };
	size_t write(const Buffer& buffer);
	size_t readSome(Buffer* buffer);
};

}
