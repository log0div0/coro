
#pragma once

#include "coro/StreamSocket.h"

#include "coro_extra/StreamIterator.h"
#include "coro_extra/Buffer.h"

namespace coro {

class TcpSocket: public StreamSocket<asio::ip::tcp> {
public:
	using StreamSocket<asio::ip::tcp>::StreamSocket;

	typedef StreamIterator<TcpSocket> Iterator;

	Iterator iterator(Buffer& buffer) {
		return Iterator(*this, buffer);
	}

	Iterator iterator() {
		return Iterator();
	}

	size_t write(const Buffer& buffer);
	size_t readSome(Buffer* buffer);

	void shutdown(asio::ip::tcp::socket::shutdown_type what = asio::ip::tcp::socket::shutdown_both) {
		_handle.shutdown(what);
	}

	bool isOpen() {
		return _handle.is_open();
	}
};

}