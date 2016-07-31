
#pragma once

#include "coro/AsioTask.h"

namespace coro {

/// Wrapper вокруг asio::ip::udp::socket
class UdpSocket {
public:
	UdpSocket();
	UdpSocket(const asio::ip::udp::endpoint& endpoint);

	UdpSocket(UdpSocket&& other);
	UdpSocket& operator=(UdpSocket&& other);

	template <typename T>
	size_t send(const T& t, const asio::ip::udp::endpoint& endpoint) {
		AsioTask2<size_t> task;
		_handle.async_send_to(t, endpoint, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t receive(const T& t, asio::ip::udp::endpoint& endpoint) {
		AsioTask2<size_t> task;
		_handle.async_receive_from(t, endpoint, task.callback());
		return task.wait(_handle);
	}

private:
	asio::ip::udp::socket _handle;
};

}