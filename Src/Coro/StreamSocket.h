
#pragma once

#include "Coro/Stream.h"
#include "Coro/IoService.h"

namespace coro {

template <typename Protocol>
class StreamSocket: public Stream<typename Protocol::socket> {
public:
	using BaseType = Stream<typename Protocol::socket>;
	using BaseType::_handle;

	StreamSocket(): BaseType(typename Protocol::socket(*IoService::current())) {
	}
	StreamSocket(typename Protocol::socket socket): BaseType(std::move(socket)) {
	}

	StreamSocket(StreamSocket&& other): BaseType(std::move(other)) {
	}
	StreamSocket& operator=(StreamSocket&& other) {
		BaseType::operator=(std::move(other));
		return *this;
	}

	void connect(const typename Protocol::endpoint& endpoint) {
		AsioTask1 task;
		_handle.async_connect(endpoint, task.callback());
		task.wait(_handle);
	}

};

using TcpSocket = StreamSocket<asio::ip::tcp>;

}