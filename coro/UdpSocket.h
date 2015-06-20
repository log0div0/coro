
#pragma once

#include "coro/AsioTask.h"
#include "coro/Mutex.h"

class UdpSocket {
public:
	UdpSocket();
	UdpSocket(const boost::asio::ip::udp::endpoint& endpoint);

	UdpSocket(UdpSocket&& other);
	UdpSocket& operator=(UdpSocket&& other);

	template <typename T>
	size_t send(const T& t, const boost::asio::ip::udp::endpoint& endpoint) {
		std::lock_guard<Mutex> lock(_sendMutex);
		AsioTask2<size_t> task;
		_handle.async_send_to(t, endpoint, task.callback());
		return task.wait(_handle);
	}

	template <typename T>
	size_t receive(const T& t, boost::asio::ip::udp::endpoint& endpoint) {
		std::lock_guard<Mutex> lock(_receiveMutex);
		AsioTask2<size_t> task;
		_handle.async_receive_from(t, endpoint, task.callback());
		return task.wait(_handle);
	}

private:
	boost::asio::ip::udp::socket _handle;
	Mutex _receiveMutex, _sendMutex;
};